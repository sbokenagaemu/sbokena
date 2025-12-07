// helper functions for the editor window

#pragma once

#include <Window.hpp>
#include <position.hh>

using namespace sbokena::position;

// checks whether a position is in the the specified area
bool is_inside(
  raylib::Vector2 pos, raylib::Vector2 min, raylib::Vector2 max
);

// returns the the new grid offset based on the resized grid
raylib::Vector2 refactored_grid_offset(
  raylib::Vector2 offset_pos, raylib::Vector2 point_pos, float scale
);

// returns the max position in the grid
raylib::Vector2 grid_end(raylib::Vector2 offset_pos, float tile_size);

// returns the index of the tile that has the specific position
Position<>
tile_index(raylib::Vector2 pos, raylib::Vector2 offset, float size);
