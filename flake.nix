{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";

    git-hooks-nix.url = "github:cachix/git-hooks.nix";
    git-hooks-nix.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
      ];

      imports = [
        inputs.git-hooks-nix.flakeModule
      ];

      perSystem = {
        config,
        pkgs,
        ...
      }: let
        sbokena =
          pkgs.callPackage
          ./nix/sbokena.nix
          {};
        sbokena-dev = (sbokena
          .override {buildRelease = false;})
          .overrideAttrs {doCheck = true;};
      in {
        # pre-commit hooks
        pre-commit = {
          check.enable = true;
          settings.package = pkgs.prek;
          settings.hooks = {
            alejandra.enable = true;
            clang-format.enable = true;
            deadnix.enable = true;
            statix.enable = true;
          };
        };

        # default dev shell, activated by `nix develop`
        # manually or by `direnv` automatically
        devShells.default =
          pkgs.mkShell.override
          {stdenv = pkgs.clangStdenv;} {
            inherit
              (sbokena-dev)
              cmakeFlags
              env
              ;

            inputsFrom = [
              sbokena-dev
              config.pre-commit.devShell
            ];

            packages = [
              pkgs.llvmPackages.lldb
              pkgs.just
              pkgs.parallel
            ];

            shellHook = ''
              ${config.pre-commit.shellHook}
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

        # tests, run with `nix flake check`
        checks.sbokena = sbokena-dev;
      };
    };
}
