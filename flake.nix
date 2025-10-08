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

      # only tools needed to build for nix
      # for dev tools, see devShells
      buildTools = with pkgs; [
        llvm.clang
        cmake
        ninja
      ];
    in {
      formatter = pkgs.alejandra;

      devShells.default = pkgs.mkShell.override {inherit stdenv;} {
        buildInputs =
          buildTools
          ++ (with pkgs; [
            llvm.bintools
            llvm.clang-tools
            llvm.lldb
            fd
            just
          ]);
      };
    });
}
