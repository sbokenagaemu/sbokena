#pragma once

#include <concepts>
#include <limits>

#include <nlohmann/json.hpp>

#include "direction.hh"
#include "types.hh"

using nlohmann::json;

using namespace sbokena::types;
using namespace sbokena::direction;

namespace sbokena::position {

// a position in a 2D grid.
// for the game, +y points down, and +x points right.
template <std::integral T = u32>
struct Position {
  T x;
  T y;

  // this position with x and y swapped.
  [[nodiscard("tposed does not modify `this`")]]
  constexpr Position tposed() const noexcept {
    return {y, x};
  }

  // position after moving in direction dir from current position.
  [[nodiscard("move does not modify `this`")]]
  constexpr Position move(const Direction &dir) const noexcept {
    switch (dir) {
    case Direction::Up:
      return {x, y - 1};
    case Direction::Down:
      return {x, y + 1};
    case Direction::Left:
      return {x - 1, y};
    case Direction::Right:
      return {x + 1, y};
    }
  }

  // negate this position element-wise.
  [[nodiscard("operator- does not modify `this`")]]
  constexpr Position operator-() noexcept {
    return {-x, -y};
  }

  // add 2 positions element-wise.
  [[nodiscard("operator+ does not modify `this`")]]
  constexpr Position operator+(const Position<T> &rhs) noexcept {
    return {x + rhs.x, y + rhs.y};
  }

  // subtract 2 positions element-wise.
  [[nodiscard("operator- does not modify `this`")]]
  constexpr Position operator-(const Position<T> &rhs) noexcept {
    return {x - rhs.x, y - rhs.y};
  }

  // add another position element-wise.
  constexpr Position &operator+=(const Position<T> &rhs) noexcept {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  // subtract another position element-wise.
  constexpr Position &operator-=(const Position<T> &rhs) noexcept {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  // equality compare this position to another.
  constexpr bool operator==(const Position<T> &rhs) const noexcept {
    return (x == rhs.x) && (y == rhs.y);
  }

  // inequality compare this position to another.
  constexpr bool operator!=(const Position<T> &rhs) const noexcept {
    return !(*this == rhs);
  }

  // lexicographical less-than compare this position to another.
  constexpr bool operator<(const Position<T> &rhs) const noexcept {
    return x < rhs.x || (x == rhs.x && y < rhs.y);
  }
};

// the minimal `Position` value.
template <std::integral T = u32>
constexpr Position<T> POS_MIN = {
  .x = std::numeric_limits<T>::min(),
  .y = std::numeric_limits<T>::min()
};

// the maximal `Position` value.
template <std::integral T = u32>
constexpr Position<T> POS_MAX = {
  .x = std::numeric_limits<T>::max(),
  .y = std::numeric_limits<T>::max()
};

template <std::integral T>
void from_json(const json &j, Position<T> &pos) {
  j.at("x").get_to(pos.x);
  j.at("y").get_to(pos.y);
}

template <std::integral T>
void to_json(json &j, const Position<T> &pos) {
  j = json::object({
    {"x", pos.x},
    {"y", pos.y},
  });
}

} // namespace sbokena::position
