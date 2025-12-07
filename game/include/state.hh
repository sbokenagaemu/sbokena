// the game world's state machine.

#pragma once

#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "direction.hh"
#include "level.hh"
#include "position.hh"

using namespace sbokena::level;
using sbokena::direction::Direction;
using sbokena::position::Position;

namespace sbokena::game::state {

// the result of calling `State::step`.
// see that method's documentation for details.
enum struct StepResult {
  Ok,
  LevelComplete,
  // invalid movements
  HitWall,
  SlamOnDoor,
  InvalidDirection,
  PushTwoObjects,
  PushYourself
};

// pair of position of door and its corresponding buttons' position
using DoorSet    = std::pair<Position<>, std::vector<Position<>>>;
using PortalPair = std::pair<Position<>, Position<>>;

// the state machine.
struct State {
  std::vector<Position<>>      goal;
  std::map<Position<>, Tile>   tiles;
  std::map<Position<>, Object> objects;

  // doors and buttons grouped by id
  std::unordered_map<u32, DoorSet> doors;
  // portals grouped by id
  std::unordered_map<u32, PortalPair> portals;

  // try to advance the world state by one step, using an input.
  //
  // if this returns `Ok`, the world state has changed
  // successfully.
  //
  // otherwise, the world state has not changed, and the return value
  // can be used to determine the reason.
  StepResult step(const Direction &input) noexcept;
};

} // namespace sbokena::game::state
