// common types & aliases.

#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>

namespace sbokena::types {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using usize = std::size_t;

using f32 = std::float_t;
using f64 = std::double_t;

// a 2D cardinal direction.
enum struct Direction : u8 { Up, Down, Left, Right };

// a position in a 2D grid.
template <std::integral T>
struct Position {
  T x;
  T y;

  [[nodiscard("transposed() does not modify `this`")]]
  constexpr Position transposed() const;
};

template <std::integral T>
constexpr Position<T> &operator+=(Position<T> &, const Position<T> &) noexcept;

template <std::integral T>
constexpr Position<T> &operator-=(Position<T> &, const Position<T> &) noexcept;

template <std::integral T>
[[nodiscard("operator+() does not modify `this`")]]
constexpr Position<T> operator+(const Position<T> &,
                                const Position<T> &) noexcept;

template <std::integral T>
[[nodiscard("operator-() does not modify `this`")]]
constexpr Position<T> operator-(const Position<T> &,
                                const Position<T> &) noexcept;

template <std::signed_integral T>
[[nodiscard("operator-() does not modify `this`")]]
constexpr Position<T> operator-(const Position<T> &) noexcept;

} // namespace sbokena::types
