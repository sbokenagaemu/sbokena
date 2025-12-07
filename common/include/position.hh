#pragma once

#include <concepts>
#include <limits>

#include <nlohmann/json.hpp>

#include "types.hh"

using nlohmann::json;

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

  // lexicographical less-than compare this position to
  // another.
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
