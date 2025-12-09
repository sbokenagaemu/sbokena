// the game world's state machine.

#pragma once

#include <utility>

#include <raylib.h>

#include "direction.hh"
#include "level.hh"
#include "loader.hh"
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

using Level   = sbokena::loader::Level<Texture>;
using DoorSet = Level::DoorSet;
using Goals   = Level::Goals;
using Tiles   = Level::Tiles;
using Objects = Level::Objects;
using Doors   = Level::Doors;
using Portals = Level::Portals;

// the raw state machine.
//
// this type assumes the inner data is valid.
// if validation is needed, see `State
struct RawState {
  // try to advance the world state by one step, using an input.
  //
  // if this returns `Ok`, the world state has changed
  // successfully.
  //
  // otherwise, the world state has not changed, and the return value
  // can be used to determine the reason.
  StepResult step(const Direction &input);

  // query current points.
  usize points_query() const;

  // find player position in objects map.
  Position<> find_player() const;

  // given position, return object at that position.
  std::optional<Object> find_object(Position<> pos) const;

  // given position, return tile at that position.
  std::optional<Tile> find_tile(Position<> pos) const;

  // return both exit position from Portal and direction of exit.
  std::pair<Position<>, Direction>
  get_portal_exit(Position<> portal_from, u32 id) const;

  // check if a door is opened, aka. one of its corresponding button
  // is being pressed.
  bool is_door_open(u32 id) const;

  // check if user moves in the correct direction.
  bool is_valid_dir(const Tile &tile, Direction dir) const;

  // check if DirBox is pushed in correct direction.
  bool is_valid_dir(const Object &box, Direction step) const;

  const Goals goals;
  const Tiles tiles;
  Objects     objects;

  // doors and buttons grouped by id.
  const Doors doors;

  // portals grouped by id.
  const Portals portals;

private:
  // update the position of object in the map.
  void update_position(Position<> from, Position<> to);

  // update object's position if possible. else return the error.
  std::optional<StepResult>
  move_object(Direction dir, Position<> from, Position<> to);
};

// a known valid state machine.
class State {
public:
  // create a `State` from a `Level`.
  State(const Level &);

  State()                         = delete;
  State(const State &)            = default;
  State &operator=(const State &) = delete;
  State(State &&)                 = delete;
  State &operator=(State &&)      = delete;
  ~State()                        = default;

  // the inner `RawState`.
  const RawState &inner() const noexcept;

private:
  RawState state_;
};

} // namespace sbokena::game::state
