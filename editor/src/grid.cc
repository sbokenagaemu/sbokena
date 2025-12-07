#include "grid.hh"

using namespace sbokena::position;

// checks whether a position is in the the specified area
bool is_inside(
  raylib::Vector2 pos, raylib::Vector2 min, raylib::Vector2 max
) {
  // checks maxes
  if (pos.GetX() <= min.GetX() || pos.GetY() <= min.GetY())
    return false;
  // checks mins
  if (pos.GetX() >= max.GetX() || pos.GetY() >= max.GetY())
    return false;
  return true;
}

// returns the the new grid offset based on the resized grid
raylib::Vector2 refactored_grid_offset(
  raylib::Vector2 offset_pos, raylib::Vector2 point_pos, float scale
) {
  const raylib::Vector2 distance = point_pos - offset_pos;
  return offset_pos + (distance - (distance.Scale(scale)));
};

// returns the max position in the grid
raylib::Vector2
grid_end(raylib::Vector2 offset_pos, float tile_size) {
  return {
    offset_pos.GetX() + 32 * tile_size,
    offset_pos.GetY() + 32 * tile_size
  };
}

// returns the index of the tile that has the specific position
Position<>
tile_index(raylib::Vector2 pos, raylib::Vector2 offset, float size) {
  unsigned int x = floorf((pos.GetX() - offset.GetX()) / size);
  unsigned int y = floorf((pos.GetY() - offset.GetY()) / size);
  return {x, y};
}
