#pragma once

#include <array>

#include <Vector2.hpp>
#include <raylib.h>

#include "types.hh"

using namespace sbokena::types;

// returns the vector that the mouse followed
raylib::Vector2 mouse_position_difference(raylib::Vector2 pastMousePos) {
  return Vector2Subtract(GetMousePosition(), pastMousePos);
}

class Grid {
public:
  Grid();
  // changes the direction of a specific tile
  void change_tile_dir(int x, int y, Directions dir);

  // changes the theme of the grid
  void change_id(int id);

private:
  // stores each tile (x, y) -> (texture, direction)
  std::array<std::array<std::pair<int, sbokena::types::Directions>, 32>, 32>
    grid_;

  // the id of the theme currently in use
  int id_;
};
