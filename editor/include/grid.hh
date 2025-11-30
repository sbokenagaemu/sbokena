//

#pragma once

#include <Vector2.hpp>

#include "types.hh"

bool is_inside(
  raylib::Vector2 pos, raylib::Vector2 min, raylib::Vector2 max
);

raylib::Vector2 refactored_grid_offset(
  raylib::Vector2 offset_pos, raylib::Vector2 point_pos, float scale
);

raylib::Vector2 grid_end(raylib::Vector2 offset_pos, float tile_size);

raylib::Vector2
tile_index(raylib::Vector2 pos, raylib::Vector2 offset, float size);
