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

// a set of `Direction`.
class Directions {
public:
  // construct an empty direction set.
  constexpr Directions() noexcept : flags_ {0} {}

  // construct a direction set from a direction.
  constexpr Directions(const Direction &dir) noexcept
    : flags_ {static_cast<u8>(dir)} {}

  // construct a direction set from a raw flag value.
  // this zeroes the upper bits of the value.
  constexpr Directions(u8 flags) noexcept : flags_ {flags} {}

  // does this set contain some cardinal direction?
  constexpr bool contains(const Direction &dir) const noexcept {
    return flags_ & static_cast<u8>(dir);
  }

  // does this set contain all of these directions?
  constexpr bool contains_all(const Directions &dirs) const noexcept {
    return dirs.empty() || (*this & dirs) == dirs;
  }

  // does this set contain any of these directions?
  constexpr bool contains_any(const Directions &dirs) const noexcept {
    return dirs.empty() || (*this & dirs);
  }

  // is this set empty?
  constexpr bool empty() const noexcept {
    return !flags_;
  }

  // is this set not empty?
  constexpr operator bool() const noexcept {
    return !empty();
  }

  // return the raw flag value.
  constexpr u8 flags() const noexcept {
    return flags_;
  }

  // equality compare 2 direction sets.
  [[nodiscard("operator== does not modify arguments")]]
  constexpr bool operator==(const Directions &dirs) const noexcept {
    return flags_ == dirs.flags_;
  }

  // inequality compare 2 direction sets.
  [[nodiscard("operator!= does not modify arguments")]]
  constexpr bool operator!=(const Directions &dirs) const noexcept {
    return !(*this == dirs);
  }

  // the union of this direction set with a direction.
  [[nodiscard("operator| does not modify arguments")]]
  constexpr Directions
  operator|(const Direction &dir) const noexcept {
    return {static_cast<u8>(flags_ | static_cast<u8>(dir))};
  }

  // the intersection of this direction set with a direction.
  [[nodiscard("operator& does not modify arguments")]]
  constexpr Directions
  operator&(const Direction &dir) const noexcept {
    return {static_cast<u8>(flags_ & static_cast<u8>(dir))};
  }

  // the union of 2 direction sets.
  [[nodiscard("operator| does not modify arguments")]]
  constexpr Directions
  operator|(const Directions &dirs) const noexcept {
    return {static_cast<u8>(flags_ | dirs.flags_)};
  }

  // the intersection of 2 direction sets.
  [[nodiscard("operator& does not modify arguments")]]
  constexpr Directions
  operator&(const Directions &dirs) const noexcept {
    return {static_cast<u8>(flags_ & dirs.flags_)};
  }

  // add a direction to this direction set.
  constexpr Directions &operator|=(const Direction &dir) noexcept {
    *this = *this | dir;
    return *this;
  }

  // union another direction set to this one.
  constexpr Directions &operator|=(const Directions &dirs) noexcept {
    *this = *this | dirs;
    return *this;
  }

  // intersect a direction with this direction set.
  constexpr Directions &operator&=(const Direction &dir) noexcept {
    *this = *this & dir;
    return *this;
  }

  // intersect another direction set to this one.
  constexpr Directions &operator&=(const Directions &dirs) noexcept {
    *this = *this & dirs;
    return *this;
  }

private:
  u8 flags_ = 0b0000;
};

// union 2 directions together into a set.
[[nodiscard("operator| does not modify arguments")]]
constexpr Directions
operator|(const Direction &lhs, const Direction &rhs) noexcept {
  return {
    static_cast<u8>(static_cast<u8>(lhs) | static_cast<u8>(rhs))
  };
}

} // namespace sbokena::direction
