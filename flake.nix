{
  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
      ];

      perSystem = {
        self',
        pkgs,
        ...
      }: let
        sbokena =
          pkgs.callPackage
          ./nix/sbokena.nix
          {};
      in {
        # Nix formatter, run with `nix fmt`
        formatter = pkgs.writeShellScriptBin "nix-fmt" ''
          ${pkgs.alejandra}/bin/alejandra -q .
        '';

        # default dev shell, activated by `nix develop`
        # manually or by `direnv` automatically
        devShells.default =
          pkgs.mkShell.override
          {stdenv = pkgs.clangStdenv;} {
            inherit (sbokena.passthru) env;

            inputsFrom = [
              sbokena
            ];

            packages = [
              pkgs.llvmPackages.lldb
              pkgs.just
              pkgs.parallel
            ];

            shellHook = ''
              if [ ! -d ./build ]; then
                just cmake
              fi
            '';
          };

        # buildable packages, by `nix build .#<name>`
        # also includes checks specific to the package
        packages = {
          inherit sbokena;
          default = sbokena;

          sbokena-wayland = sbokena
            .override {enableX11 = false;};
          sbokena-x11 = sbokena
            .override {enableWayland = false;};
        };

        # miscellaneous checks, run with `nix flake check`
        checks = {
          format =
            pkgs.callPackage
            ./nix/checks/format.nix
            {};

          sbokena = (sbokena
            .override {buildRelease = false;})
            .overrideAttrs {doCheck = true;};
        };
      };
    };
}
