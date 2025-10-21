#include "types.hh"

#include <concepts>

namespace sbokena::types {

constexpr Directions::Directions(const Direction &dir) noexcept
  : flags_{static_cast<u8>(dir)} {}

constexpr Directions::Directions(u8 flags) noexcept
  : flags_{static_cast<u8>(flags & 0b1111)} {}

constexpr bool Directions::contains(const Direction &dir) const noexcept {
  return flags_ & static_cast<u8>(dir);
}

constexpr bool Directions::contains_all(const Directions &dirs) const noexcept {
  return (flags_ & dirs.flags_) == dirs.flags_;
}

constexpr bool Directions::contains_any(const Directions &dirs) const noexcept {
  return flags_ & dirs.flags_;
}

constexpr bool Directions::empty() const noexcept {
  return !flags_;
}

constexpr u8 Directions::flags() const noexcept {
  return flags_;
}

constexpr Directions operator|(const Direction &lhs,
                               const Direction &rhs) noexcept {
  return {static_cast<u8>(static_cast<u8>(lhs) | static_cast<u8>(rhs))};
}

constexpr Directions operator|(const Directions &lhs,
                               const Direction &rhs) noexcept {
  return {static_cast<u8>(lhs.flags() | static_cast<u8>(rhs))};
}

constexpr Directions operator|(const Directions &lhs,
                               const Directions &rhs) noexcept {
  return {static_cast<u8>(lhs.flags() | rhs.flags())};
}

constexpr Directions &operator|=(Directions &lhs,
                                 const Direction &rhs) noexcept {
  lhs = lhs | rhs;
  return lhs;
}

constexpr Directions &operator|=(Directions &lhs,
                                 const Directions &rhs) noexcept {
  lhs = lhs | rhs;
  return lhs;
}

template <std::integral T>
constexpr Position<T> Position<T>::transposed() const noexcept {
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
