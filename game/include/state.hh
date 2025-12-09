// the game world's state machine.

#pragma once

#include <utility>

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


// the state machine.
struct State {
  const Goals goals;
  const Tiles tiles;
  Objects     objects;

  // doors and buttons grouped by id
  const Doors doors;
  // portals grouped by id
  const Portals portals;

  // try to advance the world state by one step, using an input.
  //
  // if this returns `Ok`, the world state has changed
  // successfully.
  //
  // otherwise, the world state has not changed, and the return value
  // can be used to determine the reason.
  StepResult step(const Direction &input);

private:
  // find player position in objects map.
  Position<> find_player() const;

  // given position, return object at that position.
  std::optional<Object> find_object(Position<> pos) const;

  // given position, return tile at that position.
  std::optional<Tile> find_tile(Position<> pos) const;

  // return both exit position from Portal and direction of exit.
  std::pair<Position<>, Direction>
  get_portal_exit(Position<> portal_from, u32 id) const;

  // query current points.
  usize points_query() const;

  // check if a door is opened, aka. one of its corresponding button
  // is being pressed.
  bool is_door_open(u32 id) const;

  // check if user moves in the correct direction.
  bool is_valid_dir(const Tile &tile, Direction dir) const;

  // check if DirBox is pushed in correct direction.
  bool is_valid_dir(const Object &box, Direction step) const;

  // update the position of object in the map.
  void update_position(Position<> from, Position<> to);

  // update object's position if possible. else return the error.
  std::optional<StepResult>
  move_object(Direction dir, Position<> from, Position<> to);
};

} // namespace sbokena::game::state
