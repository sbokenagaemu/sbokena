#pragma once

#include <Vector2.hpp>
#include <map>
#include <raylib.h>
#include <utility>

#include "direction.hh"
#include "position.hh"

using namespace sbokena::direction;
using namespace sbokena::position;

// returns the vector that the mouse followed
raylib::Vector2 mouse_position_difference(raylib::Vector2 pastMousePos) {
  return Vector2Subtract(GetMousePosition(), pastMousePos);
}

class Grid {
public:
  Grid();
  // changes the direction of a specific tile
  void change_tile_dir(Position<>, Directions dir);

  // changes the theme of the grid
  void change_id(int id);

private:
  // stores each tile (Position) -> (texture_id [0-16], direction)
  std::map<Position<>, std::pair<int, Directions>> grid_;

  // the id of the theme currently in use
  int theme_id_;
};
