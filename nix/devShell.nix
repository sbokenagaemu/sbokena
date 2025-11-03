{pkgs, ...}: let
  llvm = pkgs.llvmPackages;
  inherit (llvm) stdenv;

  sbokena =
    pkgs.callPackage
    ./sbokena.nix
    {};
in
  pkgs.mkShell {inherit stdenv;} {
    inputsFrom = [sbokena];
    packages = [llvm.lldb];

    inherit (sbokena) LD_LIBRARY_PATH;
  }
