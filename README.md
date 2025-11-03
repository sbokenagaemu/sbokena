# sbokena

just a silly little Sokoban-style game.

## building

### with Nix

```sh
nix build -L
# run the game
nix run --impure github:nix-community/nixGL -- result/game/game
# run the level editor
nix run --impure github:nix-community/nixGL -- result/editor/editor
```

### without Nix

install the required dependencies for your distribution:

| Distribution | Command                                                                                                                                                                                                    |
| ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Arch Linux   | `sudo pacman -S base-devel lld cmake fd just ninja libglvnd wayland libxkbcommon wayland-protocols libx11 libxcursor libxi libxinerama libxrandr`                                                          |
| Ubuntu       | `sudo apt install build-essential git lld cmake fd-find just ninja-build libglvnd-dev libwayland-dev libxkbcommon-dev wayland-protocols libx11-dev libxcursor-dev libxi-dev libxinerama-dev libxrandr-dev` |

and build the game:

```sh
just build -DCMAKE_BUILD_TYPE=Release
build/game/game # run the game
build/editor/editor # run the level editor
```

if you want a Wayland-only build, you should also pass
`-DGLFW_BUILD_WAYLAND=1` and `-DGLFW_BUILD_X11=0`, and vice
versa for X11-only builds _(why.)_

## hacking

the Nix flake has a dev shell with everything set up.

```sh
nix develop
```

if you're not on NixOS, you may need to either symlink your
GPU driver into `/run/opengl-driver`, or use `nixGL`
(recommended):

```sh
nix profile add --impure github:nix-community/nixGL
```

after which you can run the built binaries with

```sh
nixGL result/game/game # run the game
nixGL result/editor/editor # run the level editor
```

remember to also read the
[contributors' guide](./CONTRIBUTING.md).
