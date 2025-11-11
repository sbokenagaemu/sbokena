{
  # nixpkgs
  callPackage,
  mkShell,
  clangStdenv,
  llvmPackages,
  # dependencies
  just,
  ...
}: let
  sbokena =
    callPackage
    ./sbokena.nix
    {};
in
  mkShell.override
  {stdenv = clangStdenv;} {
    inputsFrom = [sbokena];

    packages = [
      llvmPackages.lldb
      just
    ];

    shellHook = ''
      just cmake
    '';

    inherit (sbokena) LD_LIBRARY_PATH;
  }
