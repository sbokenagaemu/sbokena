#include "types.hh"

#include <concepts>

namespace sbokena::types {

Directions::Directions() noexcept : flags_{0} {}

Directions::Directions(const Direction &dir) noexcept
  : flags_{static_cast<u8>(dir)} {}

Directions::Directions(u8 flags) noexcept
  : flags_{static_cast<u8>(flags & 0b1111)} {}

bool Directions::contains(const Direction &dir) const noexcept {
  return flags_ & static_cast<u8>(dir);
}

bool Directions::contains_all(const Directions &dirs) const noexcept {
  return !dirs.flags_ || (flags_ & dirs.flags_) == dirs.flags_;
}

bool Directions::contains_any(const Directions &dirs) const noexcept {
  return !dirs.flags_ || (flags_ & dirs.flags_);
}

bool Directions::empty() const noexcept {
  return !flags_;
}

u8 Directions::flags() const noexcept {
  return flags_;
}

Directions operator|(const Direction &lhs, const Direction &rhs) noexcept {
  return {static_cast<u8>(static_cast<u8>(lhs) | static_cast<u8>(rhs))};
}

Directions operator|(const Directions &lhs, const Direction &rhs) noexcept {
  return {static_cast<u8>(lhs.flags() | static_cast<u8>(rhs))};
}

Directions operator|(const Directions &lhs, const Directions &rhs) noexcept {
  return {static_cast<u8>(lhs.flags() | rhs.flags())};
}

Directions &operator|=(Directions &lhs, const Direction &rhs) noexcept {
  lhs = lhs | rhs;
  return lhs;
}

Directions &operator|=(Directions &lhs, const Directions &rhs) noexcept {
  lhs = lhs | rhs;
  return lhs;
}

template <std::integral T>
Position<T> Position<T>::transposed() const noexcept {
  return {y, x};
}

template <std::integral T>
Position<T> operator+(const Position<T> &lhs, const Position<T> &rhs) noexcept {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <std::integral T>
Position<T> operator-(const Position<T> &lhs, const Position<T> &rhs) noexcept {
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template <std::integral T>
Position<T> &operator+=(Position<T> &lhs, const Position<T> &rhs) noexcept {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

template <std::integral T>
Position<T> &operator-=(Position<T> &lhs, const Position<T> &rhs) noexcept {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

template <std::signed_integral T>
Position<T> operator-(const Position<T> &pos) noexcept {
  return {-pos.x, -pos.y};
}

} // namespace sbokena::types
