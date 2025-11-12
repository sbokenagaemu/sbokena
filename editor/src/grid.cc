#include "grid.hh"
#include "types.hh"

using namespace sbokena::types;

void Grid::change_tile_dir(int x, int y, Directions dir) {
  Grid::grid_[x][y] = {Grid::grid_[x][y].first, dir};
}

void Grid::change_id(int id) {
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      Grid::grid_[i][j] = {id, Grid::grid_[i][j].second};
    }
  }
}

Grid::Grid() : id_{0} {
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      Grid::grid_[i][j] = {0, Direction::Up};
    }
  }
}
