// common utility types.

#pragma once

#include <optional>

namespace sbokena::utils {

// RAII scope guard that calls a function when destroyed.
//
// This is intended to be used as an unnamed variable `Deferred _{fn}` in most
// cases, or as a named variable `Deferred df{fn}` where cancellation is needed.
template <std::invocable F>
class Deferred {
public:
  // create a deferred scope guard with an invocable.
  constexpr explicit Deferred(F fn) noexcept : fn{fn} {}

  // don't call the invocable when the guard is destroyed.
  constexpr void cancel() {
    fn.reset();
  }

  constexpr ~Deferred() {
    if (fn.has_value())
      fn.value()();
  }

private:
  std::optional<F> fn;
};

} // namespace sbokena::utils
