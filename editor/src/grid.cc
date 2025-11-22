#include "grid.hh"
#include "position.hh"

using namespace sbokena::direction;
using namespace sbokena::position;

void Grid::change_tile_dir(Position<> pos, Directions dir) {
  Grid::grid_[pos] = {Grid::grid_[pos].first, dir};
}

void Grid::change_id(int id) {
  for (unsigned int i = 0; i < 32; i++) {
    for (unsigned int j = 0; j < 32; j++) {
      Grid::grid_[Position<>{i, j}] = {id,
                                       Grid::grid_[Position<>{i, j}].second};
    }
  }
}

Grid::Grid() : theme_id_{0} {
  for (unsigned int i = 0; i < 32; i++) {
    for (unsigned int j = 0; j < 32; j++) {
      Grid::grid_[Position<>{i, j}] = {0, Direction::Up};
    }
  }
}
