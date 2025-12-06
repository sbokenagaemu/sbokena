// common utility types.

#pragma once

#include <array>
#include <exception>
#include <filesystem>
#include <optional>
#include <span>
#include <stdexcept>
#include <variant>

#include <nfd.h>
#include <nfd.hpp>

namespace fs = std::filesystem;

namespace sbokena::utils {

// ===== language utils =====

// RAII scope guard that calls a function when destroyed.
//
// This is intended to be used as an unnamed variable
// `Deferred _{fn}` in most cases, or as a named variable
// `Deferred df{fn}` where cancellation is needed.
//
// See detailed usage examples in `tests/deferred_test.cc`.
template <std::invocable F>
class Deferred {
public:
  // create a deferred scope guard with an invocable.
  constexpr explicit Deferred(F fn) noexcept : fn {fn} {}

  // don't call the invocable when the guard is destroyed.
  constexpr void cancel() noexcept {
    fn.reset();
  }

  constexpr ~Deferred() {
    if (fn)
      fn.value()();
  }

private:
  std::optional<F> fn;
};

// "the overload trick"
// used with `std::visit` called on a `std::variant` to run
// one of several functions on its alternatives.
template <typename... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

// get the index of a type in a `std::variant`, or its size
// if the type is not one of its members.
template <typename V, typename T, size_t I = 0>
constexpr size_t index_of() {
  using V_I = std::variant_alternative_t<I, V>;

  if constexpr (I >= std::variant_size_v<V>)
    return std::variant_size_v<V>;
  else if constexpr (std::is_same_v<V_I, T>)
    return I;
  else
    return index_of<V, T, I + 1>();
}

// clang-format off

// assert a predicate, or throw an exception.
constexpr void assert_throw(
  bool pred,
  const std::exception &ex =
    std::runtime_error {"assertion failed"}
) {
  if (!pred)
    throw ex;
}

// clang-format on

// ===== file UI =====

// file dialog filter for level file types.
constexpr std::array<nfdfilteritem_t, 1> LEVEL_FILTER = {{
  {.name = "sbokena level", .spec = "sb,sbk"},
}};

// create a file dialog for user to open a file.
std::optional<fs::path> open_file_dialog(
  std::span<const nfdfilteritem_t> filter = LEVEL_FILTER
);

// create a folder dialog for user to open a folder.
std::optional<fs::path> open_folder_dialog();

// save a file and return its location.
std::optional<fs::path> save_file_dialog(
  std::span<const nfdfilteritem_t> filter = LEVEL_FILTER
);

} // namespace sbokena::utils
