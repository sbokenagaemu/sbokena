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

} // namespace sbokena::position
