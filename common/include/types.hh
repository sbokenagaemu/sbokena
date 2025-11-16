// common types aliases.

#pragma once

#include <cmath>
#include <cstdint>
#include <cstddef>  // for size_t

namespace sbokena::types {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

#if defined(__APPLE__)
using usize = size_t;  // macOS: global namespace
#else
using usize = std::size_t;  // Linux/other platforms
#endif

using f32 = std::float_t;
using f64 = std::double_t;

} // namespace sbokena::types
