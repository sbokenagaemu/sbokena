{
  # nixpkgs
  callPackage,
  mkShell,
  clangStdenv,
  llvmPackages,
  # dependencies
  just,
  parallel,
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
      parallel
    ];

    shellHook = ''
      just cmake
    '';

    inherit (sbokena.passthru) env;
  }
