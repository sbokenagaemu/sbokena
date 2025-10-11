{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {inherit system;};
      llvm = pkgs.llvmPackages_latest;
      xorg = pkgs.xorg;

      raylib-src = builtins.fetchGit {
        url = "https://github.com/raysan5/raylib";
        rev = "c1ab645ca298a2801097931d1079b10ff7eb9df8";
      };

      stdenv = llvm.stdenv;

      build-tools = [
        llvm.lld
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
              llvm.bintools
              llvm.clang-tools
              llvm.lldb
            ];
        };

      packages = rec {
        default = main;

        # this does not build with llvm.stdenv :shrug:
        main = pkgs.stdenv.mkDerivation {
          src = ./.;
          name = "main";
          nativeBuildInputs =
            build-tools
            ++ libs;

          configurePhase = ''
            export raylib_src=${raylib-src}
          '';

          cmakeFlags = [
            "-Draylib_src=${raylib-src}"
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
            ++ libs
            ++ [llvm.clang-tools];

          dontUseCmakeConfigure = true;
          buildPhase = ''
            just build
            just lint
          '';

          installPhase = ''
            touch $out
          '';
        };
      };
    });
}
