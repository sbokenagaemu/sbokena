#include "state.hh"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <variant>

#include "level.hh"
#include "utils.hh"

using namespace sbokena::utils;

namespace sbokena::game::state {

// find player position in objects map.
static constexpr Position<>
find_player(std::map<Position<>, Object> &objects) {
  auto iter =
    std::find_if(objects.begin(), objects.end(), [](const auto &p) {
      return std::holds_alternative<Player>(p.second);
    });

  Position<> pos = iter->first;
  return pos;
}

static constexpr std::optional<Tile>
find_tile(const std::map<Position<>, Tile> &tiles, Position<> &pos) {
  auto tile_iter = tiles.find(pos);
  if (tile_iter == tiles.end())
    return std::nullopt;
  auto &[_, tile] = *tile_iter;
  return std::optional<Tile>(tile);
}

// only find objects (Box/DirBox).
static constexpr std::optional<Object>
find_object(std::map<Position<>, Object> &objects, Position<> &pos) {
  auto obj_iter = objects.find(pos);
  if (obj_iter == objects.end())
    return std::nullopt;
  auto &[_, obj] = *obj_iter;
  // throw exception if object is Player
  assert_throw(std::holds_alternative<Player>(obj));
  return std::optional<Object>(obj);
}

// update the position of player (and object, if available) in the map
static void update_position(
  const Position<>              from,
  const Position<>              to,
  std::optional<Position<>>     object_to,
  std::map<Position<>, Object> &objects
) {
  // Position<> from is guaranteed to be
  // in the map when player is found
  if (object_to) {
    auto object_handler  = objects.extract(to);
    object_handler.key() = object_to.value();
    objects.insert(std::move(object_handler));
  }

  auto player_handler  = objects.extract(from);
  player_handler.key() = to;
  objects.insert(std::move(player_handler));
}

// check if a door is opened, aka. one of its corresponding button is
// being pressed. return nullopt if no door of id is found.
static bool is_door_open(
  const u32                               id,
  const std::unordered_map<u32, DoorSet> &doors,
  // FIXME: should be const but currently not const
  std::map<Position<>, Object> &objects
) {
  auto door_iter = doors.find(id);
  assert_throw(door_iter != doors.end());
  auto buttons = door_iter->second.second;
  for (auto b_pos : buttons)
    if (find_object(objects, b_pos).has_value())
      return false;
  return true;
}

// check if user steps in to DirFloor/Portal in the correct
// direction.
static bool is_valid_dir(const Tile &tile, const Direction &step) {
  if (std::holds_alternative<DirFloor>(tile))
    return std::get<DirFloor>(tile).dir == step;
  else
    return std::get<Portal>(tile).in_dir == step;
}

static bool is_valid_dir(const DirBox &box, const Direction &step) {
  return box.dir == step;
}

// return both exit position and direction of exit
static std::pair<Position<>, Direction> get_portal_exit(
  const Position<>                           portal_from,
  const u32                                  id,
  const std::map<Position<>, Tile>          &tiles,
  const std::unordered_map<u32, PortalPair> &portals
) {
  auto portal_iter = portals.find(id);
  assert_throw(
    portal_iter != portals.end(),
    std::logic_error {"portal not found"}
  );
  auto       portal_pair = portal_iter->second;
  Position<> portal_to;
  if (portal_from == portal_pair.first)
    portal_to = portal_pair.second;
  else
    portal_to = portal_pair.first;
  Tile      portal_to_ = find_tile(tiles, portal_to).value();
  Direction out_dir    = -std::get<Portal>(portal_to_).in_dir;
  return std::pair(portal_to.move(out_dir), out_dir);
}

// check type of the tile that player will take over from the object
// TODO: check that input is correct even when called from only moving
// player through portal
static std::optional<StepResult> move_player_object(
  const Direction              &input,
  const Tile                   &player_tile,
  Position<>                    player_from,
  Position<>                    player_to,
  Position<>                    box_to,
  std::map<Position<>, Object> &objects
) {
  switch (player_tile.index()) {
  case index_of<Tile, DirFloor>(): {
    if (!is_valid_dir(player_tile, input))
      return std::optional<StepResult>(StepResult::InvalidDirection);
    // falls through
  }
  case index_of<Tile, Goal>(): {
    update_position(player_to, box_to, objects);
    update_position(player_from, player_to, objects);
    break;
  }
    // player moving to the Goal containing a Box already, meaning
    // that number of goals met decreases by 1

  case index_of<Tile, Floor>():
  case index_of<Tile, Door>():
  case index_of<Tile, Button>(): {
    update_position(player_to, box_to, objects);
    update_position(player_from, player_to, objects);
    break;
  }
  }
  return std::nullopt;
}

// move player and the object in the next tile
static constexpr std::optional<StepResult> move_objects(
  const Direction &input,
  Position<>       player_from,
  Position<>       player_to,
  Object           object,
  const Tile      &player_tile, // tile that the player will move to
  std::map<Position<>, Object>              &objects,
  std::map<Position<>, Tile>                &tiles,
  const std::unordered_map<u32, DoorSet>    &doors,
  const std::unordered_map<u32, PortalPair> &portals

) {
  // objects can not be placed on portal
  assert_throw(
    std::holds_alternative<level::Portal>(player_tile),
    std::logic_error {"Object cannot be on Portal"}
  );

  // check direction for DirBox
  if (std::holds_alternative<DirBox>(object)) {
    if (!is_valid_dir(std::get<DirBox>(object), input))
      return StepResult::InvalidDirection;
  }

  Position<> object_to    = player_to.move(input);
  auto       object_tile_ = find_tile(tiles, object_to);
  if (!object_tile_)
    return StepResult::StepOnWall;
  auto object_tile = object_tile_.value();

  auto object_object = find_object(objects, object_to);
  if (object_object)
    return StepResult::PushTwoBoxes;

  switch (object_tile.index()) {
  case index_of<Tile, Floor>():
  case index_of<Tile, Button>():
  case index_of<Tile, Door>(): {
    if (!is_door_open(
          std::get<Door>(object_tile).door_id, doors, objects
        ))
      return std::optional<StepResult>(StepResult::SlamOnDoor);
  }
  case index_of<Tile, Goal>(): {
    return move_player_object(
      input, player_tile, player_from, player_to, object_to, objects
    );
    break;
  }
  case index_of<Tile, Portal>(): {
    if (!is_valid_dir(object_tile, input))
      return std::optional<StepResult>(StepResult::InvalidDirection);
    auto [p_portal_exit, out_dir] = get_portal_exit(
      object_to,
      std::get<Portal>(object_tile).portal_id,
      tiles,
      portals
    );

    std::optional<Tile> exit_tile_ = find_tile(tiles, p_portal_exit);
    if (!exit_tile_)
      return std::optional<StepResult>(StepResult::StepOnWall);
    auto exit_tile = exit_tile_.value();

    std::optional<Object> exit_object_ =
      find_object(objects, p_portal_exit);
    if (exit_object_)
      return std::optional<StepResult>(StepResult::PushTwoBoxes);

    return move_objects(
      out_dir,
      player_from,
      p_portal_exit,
      object,
      exit_tile,
      objects,
      tiles,
      doors,
      portals
    );
    break;
  }
  case index_of<Tile, DirFloor>(): {
    if (!is_valid_dir(object_tile, input))
      return std::optional<StepResult>(StepResult::InvalidDirection);
    move_player_object(
      input, player_tile, player_from, player_to, object_to, objects
    );
    break;
  }
  }

  return std::nullopt;
}

// move only player
// if valid move, return nullopt, else return error
static constexpr std::optional<StepResult> move_player(
  // TODO: confirm that direction is only used in case of DirFloor
  const Direction                     &input,
  Position<>                           player_from,
  Position<>                           player_to,
  const Tile                          &tile,
  std::map<Position<>, Object>        &objects,
  std::map<Position<>, Tile>          &tiles,
  std::unordered_map<u32, DoorSet>    &doors,
  std::unordered_map<u32, PortalPair> &portals,
  bool                                 is_from_portal
) {
  usize tile_type = tile.index();
  switch (tile_type) {
    // if step on floor/goal/button, change position of player to
    // position of floor/goal/button.
  case index_of<Tile, Floor>():
  case index_of<Tile, Button>():
  case index_of<Tile, Goal>(): {
    // TODO: add something to record number of goals met
    update_position(player_from, player_to, std::nullopt, objects);
    break;
  }
  case index_of<Tile, Door>(): {
    auto door_open =
      is_door_open(std::get<Door>(tile).door_id, doors, objects);

    if (!door_open)
      return std::optional<StepResult>(StepResult::SlamOnDoor);
    else
      update_position(player_from, player_to, std::nullopt, objects);
    break;
  }
  case index_of<Tile, DirFloor>(): {
    // using is_from_portal to exclude the case when player get out
    // of a portal
    if (is_valid_dir(tile, input) || is_from_portal)
      update_position(player_from, player_to, std::nullopt, objects);
    else
      return std::optional<StepResult>(StepResult::InvalidDirection);
    break;
  }
  case index_of<Tile, Portal>(): {
    if (!is_valid_dir(tile, input))
      return std::optional<StepResult>(StepResult::InvalidDirection);
    auto [p_portal_exit, out_dir] = get_portal_exit(
      player_to, std::get<Portal>(tile).portal_id, tiles, portals
    );

    std::optional<Tile> exit_tile_ = find_tile(tiles, p_portal_exit);
    if (!exit_tile_)
      return std::optional<StepResult>(StepResult::StepOnWall);
    auto exit_tile = exit_tile_.value();

    std::optional<Object> exit_object_ =
      find_object(objects, p_portal_exit);
    if (exit_object_ == std::nullopt)
      return move_player(
        out_dir,
        player_from,
        p_portal_exit,
        exit_tile,
        objects,
        tiles,
        doors,
        portals,
        true
      );
    else
      // direction at this point should be dir out from portal
      // TODO: check where input param is used
      return move_objects(
        out_dir,
        player_from,
        p_portal_exit,
        exit_object_.value(),
        exit_tile,
        objects,
        tiles,
        doors,
        portals
      );

    break;
  }
  }

  return std::nullopt;
}

StepResult State::step(const Direction &input) noexcept {
  Position<> player_from = find_player(this->objects);

  // check if current tile is a uni-directional floor
  auto from_tile = find_tile(this->tiles, player_from).value();
  if (std::holds_alternative<DirFloor>(from_tile))
    if (!is_valid_dir(from_tile, input))
      return StepResult::InvalidDirection;
  auto player_to = player_from.move(input);

  // find destination tile
  auto to_tile_ = find_tile(this->tiles, player_to);

  // step on wall
  if (to_tile_ == std::nullopt)
    return StepResult::StepOnWall;
  auto to_tile = to_tile_.value();

  auto to_object_ = find_object(this->objects, player_to);
  std::optional<StepResult> res;
  if (!to_object_)
    res = move_player(
      input,
      player_from,
      player_to,
      to_tile,
      this->objects,
      this->tiles,
      this->doors,
      this->portals,
      false
    );
  else
    res = move_objects(
      input,
      player_from,
      player_to,
      to_object_.value(),
      to_tile,
      this->objects,
      this->tiles,
      this->doors,
      this->portals
    );

  // res == nullopt meaning that positions moved successfully
  if (!res)
    return StepResult::Ok;
  else
    return res.value();
}

} // namespace sbokena::game::state
