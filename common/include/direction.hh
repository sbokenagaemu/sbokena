#pragma once

#include <nlohmann/json.hpp>

#include "types.hh"

using namespace sbokena::types;

namespace sbokena::direction {

// a 2D cardinal direction.
enum struct Direction : u8 {
  // clang-format off
  Up    = 0b0001,
  Down  = 0b0010,
  Left  = 0b0100,
  Right = 0b1000,
  // clang-format on
};

// reverse a given direction.
constexpr Direction operator-(const Direction &dir) noexcept {
  switch (dir) {
  case Direction::Up:
    return Direction::Down;
  case Direction::Down:
    return Direction::Up;
  case Direction::Left:
    return Direction::Right;
  case Direction::Right:
    return Direction::Left;
  }
}

// clang-format off
NLOHMANN_JSON_SERIALIZE_ENUM(Direction, {
  {Direction::Up,    "Up"   },
  {Direction::Down,  "Down" },
  {Direction::Left,  "Left" },
  {Direction::Right, "Right"},
})
// clang-format on

} // namespace sbokena::direction
