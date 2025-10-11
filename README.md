# sbokena

just a silly little Sokoban-style game.

## building

### with Nix

```sh
nix build -L
nix run --impure github:nix-community/nixGL -- result/bin/main
```

### without Nix

install the required dependencies for your distribution:

| Distribution | Command                                                                                                    |
| ------------ | ---------------------------------------------------------------------------------------------------------- |
| Arch Linux   | `sudo pacman -S base-devel lld cmake fd just ninja libglvnd libx11 libxcursor libxi libxinerama libxrandr` |

and build the game:

```sh
just build -DCMAKE_BUILD_TYPE=Release
build/main
```

## hacking

the Nix flake has a dev shell with everything set up.

```sh
nix develop
```

to generate a `compile_commands.json` database for use with `clangd`:

```sh
just CC="clang" CXX="clang++" cmake
```

to build the game:

```sh
just CC="clang" CXX="clang++" build
```

to run the game:

```sh
nixGLIntel result/bin/main
```
