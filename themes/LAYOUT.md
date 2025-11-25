# sbokena theme directory layout

every `sbokena` level theme is organized as a directory
with resources inside. this directory has a flat structure,
with each file corresponding to an object in the game.

## textures

see `docs/RULES.md` for details on each object's name.
some tiles have 4-directional variants, in which case the
textures must have the `_n`/`_e`/`_w`/`_s` suffixes for the
pointing-up/right/left/down variants, respectively.

- `floor.png`: texture for a `Floor`.
- `button.png`: texture for a `Button`.
- `door_open.png`: texture for an open `Door`.
- `door_closed.png`: texture for a closed `Door`.
- `portal_*.png`: textures for a `Portal`.
- `dirfloor_*.png`: textures for a `Uni-directional floor`.
- `goal.png`: texture for a `Goal`.
- `wall.png`: texture for a `Wall`.
- `roof.png`: texture for a `Wall`, that is not adjacent to a a `Floor`.

- `player_*.png`: textures for a `Player`.
- `box.png`: texture for a `Box`.
- `dirbox_*.png`: textures for a `Uni-directional box`.
