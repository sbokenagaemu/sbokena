#pragma once

#include <concepts>

#include "types.hh"

using namespace sbokena::types;

namespace sbokena::position {

// a position in a 2D grid.
template <std::integral T = u32>
struct Position {
  T x;
  T y;

  // this position with x and y swapped.
  [[nodiscard("tposed does not modify `this`")]]
  constexpr Position tposed() const {
    return {y, x};
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

  // check if equal to another position.
  constexpr bool operator==(const Position<T> &rhs) const noexcept {
    return (x == rhs.x) && (y == rhs.y);
  }

  // check if different from another position.
  constexpr bool operator!=(const Position<T> &rhs) const noexcept {
    return !(*this == rhs);
  }

  // check if x of this position is less than that of the compared position.
  // compare x values first, if equal then compare y values.
  // used for sorting in std::map.
  constexpr bool operator<(const Position<T> &rhs) const noexcept {
    if (x < rhs.x)
      return true;
    if (x > rhs.x)
      return false;
    return (y < rhs.y);
  }

  // check if x of this position is more than that of the compared position.
  // compare x values first, if equal then compare y values.
  // used for sorting in std::map.
  constexpr bool operator>(const Position<T> &rhs) const noexcept {
    return !(*this < rhs);
  }
};

} // namespace sbokena::position
