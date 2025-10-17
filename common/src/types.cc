#include "types.hh"

#include <concepts>

namespace sbokena::types {

template <std::integral T>
constexpr Position<T> Position<T>::transposed() const {
  return {y, x};
}

template <std::integral T>
constexpr Position<T> operator+(const Position<T> &lhs,
                                const Position<T> &rhs) noexcept {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <std::integral T>
constexpr Position<T> operator-(const Position<T> &lhs,
                                const Position<T> &rhs) noexcept {
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template <std::integral T>
constexpr Position<T> &operator+=(Position<T> &lhs,
                                  const Position<T> &rhs) noexcept {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

template <std::integral T>
constexpr Position<T> &operator-=(Position<T> &lhs,
                                  const Position<T> &rhs) noexcept {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

template <std::signed_integral T>
constexpr Position<T> operator-(const Position<T> &pos) noexcept {
  return {-pos.x, -pos.y};
}

} // namespace sbokena::types
