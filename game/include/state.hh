// the game world's state machine.

#pragma once

namespace sbokena::game::state {

// an abstract input into the world state.
enum struct StateInput {};

// the result of calling `State::step`.
// see that method's documentation for details.
enum struct StepResult {
  Ok,
};

// the state machine.
struct State {
  // try to advance the world state by one step, using an input.
  //
  // if this returns `Ok`, the world state has changed successfully.
  //
  // otherwise, the world state has not changed, and the return value
  // can be used to determine the reason.
  constexpr StepResult step(const StateInput &) noexcept;
};

} // namespace sbokena::game::state
