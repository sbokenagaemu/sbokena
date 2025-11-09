// raylib utilities.

#pragma once

#include <raymath.h>

#include "types.hh"

using namespace sbokena::types;

namespace sbokena::rlutil {

// unary operators

constexpr Vector2 operator-(const Vector2 &v) noexcept {
  return {-v.x, -v.y};
}

constexpr Vector3 operator-(const Vector3 &v) noexcept {
  return {-v.x, -v.y, -v.z};
}

constexpr Vector4 operator-(const Vector4 &v) noexcept {
  return {-v.x, -v.y, -v.z, -v.w};
}

// element-wise binary operators

constexpr Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs) noexcept {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}
constexpr Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs) noexcept {
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}
constexpr Vector2 operator*(const Vector2 &lhs, const Vector2 &rhs) noexcept {
  return {lhs.x * rhs.x, lhs.y * rhs.y};
}
constexpr Vector2 operator/(const Vector2 &lhs, const Vector2 &rhs) noexcept {
  return {lhs.x / rhs.x, lhs.y / rhs.y};
}

constexpr Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs) noexcept {
  return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}
constexpr Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs) noexcept {
  return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}
constexpr Vector3 operator*(const Vector3 &lhs, const Vector3 &rhs) noexcept {
  return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}
constexpr Vector3 operator/(const Vector3 &lhs, const Vector3 &rhs) noexcept {
  return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
}

constexpr Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs) noexcept {
  return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
}
constexpr Vector4 operator-(const Vector4 &lhs, const Vector4 &rhs) noexcept {
  return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
}
constexpr Vector4 operator*(const Vector4 &lhs, const Vector4 &rhs) noexcept {
  return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w};
}
constexpr Vector4 operator/(const Vector4 &lhs, const Vector4 &rhs) noexcept {
  return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w};
}

// vector-scalar binary operators

constexpr Vector2 operator+(const Vector2 &v, f32 s) noexcept {
  return {v.x + s, v.y + s};
}
constexpr Vector2 operator-(const Vector2 &v, f32 s) noexcept {
  return {v.x - s, v.y - s};
}
constexpr Vector2 operator*(const Vector2 &v, f32 s) noexcept {
  return {v.x * s, v.y * s};
}
constexpr Vector2 operator/(const Vector2 &v, f32 s) noexcept {
  return {v.x / s, v.y / s};
}

constexpr Vector3 operator+(const Vector3 &v, f32 s) noexcept {
  return {v.x + s, v.y + s, v.z + s};
}
constexpr Vector3 operator-(const Vector3 &v, f32 s) noexcept {
  return {v.x - s, v.y - s, v.z - s};
}
constexpr Vector3 operator*(const Vector3 &v, f32 s) noexcept {
  return {v.x * s, v.y * s, v.z * s};
}
constexpr Vector3 operator/(const Vector3 &v, f32 s) noexcept {
  return {v.x / s, v.y / s, v.z / s};
}

constexpr Vector4 operator+(const Vector4 &v, f32 s) noexcept {
  return {v.x + s, v.y + s, v.z + s, v.w + s};
}
constexpr Vector4 operator-(const Vector4 &v, f32 s) noexcept {
  return {v.x - s, v.y - s, v.z - s, v.w - s};
}
constexpr Vector4 operator*(const Vector4 &v, f32 s) noexcept {
  return {v.x * s, v.y * s, v.z * s, v.w * s};
}
constexpr Vector4 operator/(const Vector4 &v, f32 s) noexcept {
  return {v.x / s, v.y / s, v.z / s, v.w / s};
}

}; // namespace sbokena::rlutil
