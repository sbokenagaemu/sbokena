// texture loader

#pragma once

#include <array>

#include <Texture.hpp>
#include <raylib.h>

#include "types.hh"

using namespace sbokena::types;

class TileID {
public:
  operator usize() const {
    return id_;
  }

  friend class Theme;

private:
  usize id_;

  constexpr TileID(usize id) : id_{id} {}
};

class Theme {
public:
  Theme(std::string);

  const std::string GetName() {
    return name_;
  }

private:
  static constexpr TileID ROOF = 0;
  static constexpr TileID WALL = 1;
  static constexpr TileID FLOOR = 2;
  static constexpr TileID PLAYER = 3;
  static constexpr TileID BOX = 4;
  static constexpr TileID BUTTON = 5;
  static constexpr TileID DOOR_OPEN = 6;
  static constexpr TileID DOOR_CLOSED = 7;
  static constexpr TileID DIR_FLOOR_UP = 8;
  static constexpr TileID DIR_FLOOR_RIGHT = 9;
  static constexpr TileID DIR_FLOOR_DOWN = 10;
  static constexpr TileID DIR_FLOOR_LEFT = 11;
  static constexpr TileID DIR_BOX_UP = 12;
  static constexpr TileID DIR_BOX_RIGHT = 13;
  static constexpr TileID DIR_BOX_DOWN = 14;
  static constexpr TileID DIR_BOX_LEFT = 15;
  static constexpr TileID PORTAL = 16;

  const std::string name_;

  std::array<raylib::Texture, 17> textures_;

  const raylib::Texture &tile(const TileID &id) const {
    return textures_[id];
  }
};

std::vector<std::string> valid_textures();