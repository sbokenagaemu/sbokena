{
  description = "just a silly little Sokoban-style game.";

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
        inherit (pkgs) xorg;

        llvm = pkgs.llvmPackages;
        inherit (llvm) stdenv;

        googletest-src = builtins.fetchGit {
          url = "https://github.com/google/googletest";
          ref = "v1.17.0";
          rev = "52eb8108c5bdec04579160ae17225d66034bd723";
        };
        raylib-src = builtins.fetchGit {
          url = "https://github.com/raysan5/raylib";
          rev = "c1ab645ca298a2801097931d1079b10ff7eb9df8";
        };
        raylib-cpp-src = builtins.fetchGit {
          url = "https://github.com/RobLoach/raylib-cpp";
          rev = "6d9d02cd242e8a4f78b6f971afcf992fb7a417dd";
        };

        build-tools = [
          # use this instead of llvm.lld
          # https://matklad.github.io/2022/03/14/rpath-or-why-lld-doesnt-work-on-nixos.html
          llvm.bintools

          # fixes clang-scan-deps for ninja
          # https://github.com/nixos/nixpkgs/issues/273875
          # (thank you, @opna2608:matrix.org)
          llvm.clang-tools

          pkgs.cmake
          pkgs.fd
          pkgs.just
          pkgs.ninja

          # build-time deps
          pkgs.libffi
          pkgs.pkg-config
          pkgs.wayland-scanner
        ];

        libs = [
          # raylib deps
          pkgs.libGL
          # raylib deps - wayland
          pkgs.libxkbcommon
          pkgs.wayland
          # raylib deps - x11
          xorg.libX11
          xorg.libXcursor
          xorg.libXi
          xorg.libXinerama
          xorg.libXrandr
        ];
      in rec {
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

            LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath libs;
          };

        packages = rec {
          default = sbokena;
          sbokena = stdenv.mkDerivation {
            src = ./.;
            name = "sbokena";
            nativeBuildInputs = build-tools;
            buildInputs = libs;

            configurePhase = ''
              export googletest_src=${googletest-src}
              export raylib_src=${raylib-src}
              export raylib_cpp_src=${raylib-cpp-src}
            '';

            cmakeFlags = [
              "-Dgoogletest_src=${googletest-src}"
              "-Draylib_src=${raylib-src}"
              "-Draylib_cpp_src=${raylib-cpp-src}"
            ];

            buildPhase = ''
              just build -DCMAKE_BUILD_TYPE=Release
            '';

            installPhase = ''
              runHook preInstall
              install -Dm 755 build/editor/editor $out/bin/editor
              install -Dm 755 build/game/game $out/bin/game
              runHook postInstall
            '';
          };
        };

        apps = {
          editor = {
            type = "app";
            program = "${packages.sbokena}/bin/editor";
          };

          game = {
            type = "app";
            program = "${packages.sbokena}/bin/game";
          };
        };

        checks = {
          format = stdenv.mkDerivation {
            name = "format";
            src = ./.;
            nativeBuildInputs = [
              llvm.clang-tools
            ];

            buildPhase = ''
              clang-format \
                --dry-run -Werror \
                $(fd '.cc') $(fd '.h')
            '';

            installPhase = ''
              touch $out
            '';
          };

          tidy = stdenv.mkDerivation {
            name = "tidy";
            src = ./.;
            nativeBuildInputs = build-tools;
            buildInputs = libs;

            configurePhase = ''
              export googletest_src=${googletest-src}
              export raylib_src=${raylib-src}
              export raylib_cpp_src=${raylib-cpp-src}
            '';

            cmakeFlags = [
              "-Dgoogletest_src=${googletest-src}"
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

          test = stdenv.mkDerivation {
            name = "test";
            src = ./.;
            nativeBuildInputs = build-tools;
            buildInputs = libs;

            configurePhase = ''
              export googletest_src=${googletest-src}
              export raylib_src=${raylib-src}
              export raylib_cpp_src=${raylib-cpp-src}
            '';

            cmakeFlags = [
              "-Dgoogletest_src=${googletest-src}"
              "-Draylib_src=${raylib-src}"
              "-Draylib_cpp_src=${raylib-cpp-src}"
            ];

            buildPhase = ''
              just build
              just test
            '';

            installPhase = ''
              touch $out
            '';
          };
        };
      }
    );
}
