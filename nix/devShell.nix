{pkgs, ...}: let
  llvm = pkgs.llvmPackages;

  sbokena =
    pkgs.callPackage
    ./sbokena.nix
    {};
in
  pkgs.mkShell.override
  {inherit (llvm) stdenv;} {
    inputsFrom = [sbokena];

    packages = [
      llvm.lldb
      pkgs.just
    ];
    inherit (sbokena) LD_LIBRARY_PATH;
  }
