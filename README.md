# sbokena

just a silly little Sokoban-style game.

## building

### with Nix

```sh
nix build
nix shell --impure github:nix-community/nixGL
nixGL result/bin/game   # run the game
nixGL result/bin/editor # run the editor
```

### without Nix

the Nix derivation in `nix/sbokena.nix` authoritatively describes the
required dependencies.

as of 2025/12/12, these dependencies are:

- `clang`
- `cmake`
- `dbus`
- `fd`
- `just`
- `libffi`
- `libGL` / `libglvnd`
- `libX11` (X11)
- `libXcursor` (X11)
- `libxkbcommon` (Wayland)
- `libXi` (X11)
- `libXinerama` (X11)
- `libXrandr` (X11)
- `lld`
- `ninja`
- `parallel`
- `pkg-config` (Wayland)
- `wayland` (Wayland)
- `wayland-scanner` (Wayland)

once these dependencies are installed

```sh
just build -DCMAKE_BUILD_TYPE=Release
build/game/game # run the game
build/editor/editor # run the level editor
```

Wayland and X11 support are controlled with the CMake feature flags
`GLFW_BUILD_WAYLAND` and `GLFW_BUILD_X11`. these flags are ignored if
`USE_EXTERNAL_GLFW` is set to `ON` or `IF_POSSIBLE` with a
system-installed copy of `libglfw`.

## hacking

the Nix flake has a dev shell with everything set up.

```sh
nix develop
```

if you're not on NixOS, you may need to either symlink your
GPU driver into `/run/opengl-driver`, or use `nixGL`
(recommended):

```sh
nix profile install --impure github:nix-community/nixGL
```

after which you can run the built binaries with

```sh
just build          # build the game
just test           # run the tests
just cov-report     # show a test coverage report
build/game/game     # run the game
build/editor/editor # run the level editor
```

all these commands should be prefixed with `nixGL` if you're using the
Nix shell on non-NixOS.

remember to also read the
[contributors' guide](./docs/CONTRIBUTING.md).
