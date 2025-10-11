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
      stdenv = llvm.stdenv;

      build-tools = [
        llvm.lld
        pkgs.cmake
        pkgs.ninja
      ];
    in {
      formatter = pkgs.alejandra;

      devShells.default =
        pkgs.mkShell.override {
          inherit stdenv;
        } {
          packages =
            build-tools
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
          nativeBuildInputs = build-tools;

          installPhase = ''
            mkdir -p $out/bin
            cp -r $name $out/bin
          '';
        };
      };
    });
}
