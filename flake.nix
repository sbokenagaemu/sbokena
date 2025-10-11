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

      stdenv = llvm.stdenv;

      build-tools = [
        llvm.lld
        pkgs.cmake
        pkgs.ninja
      ];

      libs = [
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
              pkgs.fd
              pkgs.just
            ];
        };

      packages = rec {
        default = main;

        # TODO: this does not build with llvm.stdenv
        main = stdenv.mkDerivation {
          src = ./.;
          name = "main";
          nativeBuildInputs =
            build-tools
            ++ libs;

          installPhase = ''
            mkdir -p $out/bin
            cp -r $name $out/bin
          '';
        };
      };

      checks = {
        format = stdenv.mkDerivation {
          src = ./.;
          name = "format";
          nativeBuildInputs = [
            llvm.clang-tools
            pkgs.fd
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
            ++ [
              llvm.clang-tools
              pkgs.fd
              pkgs.just
            ];

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
