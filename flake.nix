{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nixgl = {
      url = "github:nix-community/nixGL";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
      };
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    nixgl,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        overlays = [nixgl.overlay];
        pkgs = import nixpkgs {inherit system overlays;};
        llvm = pkgs.llvmPackages;
        xorg = pkgs.xorg;

        raylib-src = builtins.fetchGit {
          url = "https://github.com/raysan5/raylib";
          rev = "c1ab645ca298a2801097931d1079b10ff7eb9df8";
        };
        raylib-cpp-src = builtins.fetchGit {
          url = "https://github.com/RobLoach/raylib-cpp";
          rev = "6d9d02cd242e8a4f78b6f971afcf992fb7a417dd";
        };

        stdenv = llvm.stdenv;

        build-tools = [
          # use this instead of llvm.lld
          # https://matklad.github.io/2022/03/14/rpath-or-why-lld-doesnt-work-on-nixos.html
          llvm.bintools

          # fixes clang-scan-deps for ninja
          # https://github.com/NixOS/nixpkgs/issues/273875
          # (thank you, @opna2608:matrix.org)
          llvm.clang-tools

          pkgs.cmake
          pkgs.fd
          pkgs.just
          pkgs.ninja
        ];

        libs = [
          pkgs.libGL
          xorg.libX11
          xorg.libXcursor
          xorg.libXi
          xorg.libXinerama
          xorg.libXrandr
        ];
      in {
        formatter = pkgs.alejandra;

        devShells.default =
          pkgs.mkShell.override {
            inherit stdenv;
          } {
            packages =
              build-tools
              ++ libs
              ++ [
                llvm.lldb
                pkgs.nixgl.nixGLIntel
              ];
          };

        packages = rec {
          default = main;

          main = stdenv.mkDerivation {
            src = ./.;
            name = "main";
            nativeBuildInputs =
              build-tools
              ++ libs;

            configurePhase = ''
              export raylib_src=${raylib-src}
              export raylib_cpp_src=${raylib-cpp-src}
            '';

            cmakeFlags = [
              "-Draylib_src=${raylib-src}"
              "-Draylib_cpp_src=${raylib-cpp-src}"
            ];

            buildPhase = ''
              just build
            '';

            installPhase = ''
              runHook preInstall
              install -Dm 755 build/$name $out/bin/$name
              runHook postInstall
            '';
          };
        };

        checks = {
          format = stdenv.mkDerivation {
            src = ./.;
            name = "format";
            nativeBuildInputs = [
              llvm.clang-tools
            ];

            buildPhase = ''
              ls -R
              clang-format \
                --dry-run -Werror \
                $(fd '.cc') $(fd '.h')
            '';

            installPhase = ''
              touch $out
            '';
          };

          tidy = stdenv.mkDerivation {
            src = ./.;
            name = "tidy";
            nativeBuildInputs =
              build-tools
              ++ libs;

            configurePhase = ''
              export raylib_src=${raylib-src}
              export raylib_cpp_src=${raylib-cpp-src}
            '';

            cmakeFlags = [
              "-Draylib_src=${raylib-src}"
              "-Draylib_cpp_src=${raylib-cpp-src}"
            ];

            buildPhase = ''
              just cmake
              just lint
            '';

            installPhase = ''
              touch $out
            '';
          };
        };
      }
    );
}
