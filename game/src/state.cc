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
  case index_of<Tile, Floor>():
  case index_of<Tile, Goal>(): {
    break;
  }
    // player moving to the Goal containing a Box already, meaning
    // that number of goals met decreases by 1

  case index_of<Tile, Door>():
  case index_of<Tile, Button>(): {
    update_position(
      player_from,
      player_to,
      std::optional<Position<>>(box_to),
      objects
    );
    break;
  }
  }
  return std::nullopt;
}
}

// move only player
static constexpr std::optional<StepResult> move_player(
  const Direction                  &input,
  Position<>                        player_from,
  Position<>                        player_to,
  const Tile                       &tile,
  std::map<Position<>, Object>     &objects,
  std::unordered_map<u32, DoorSet> &doors
) {
  usize tile_type = tile.index();
  switch (tile_type) {
    // if step on floor/goal/button, change position of player to
    // position of floor/goal/button.
  case index_of<Tile, Floor>():
  case index_of<Tile, Goal>(): {
    update_position(player_from, player_to, objects);
    break;
  }
  case index_of<Tile, Button>(): {
    update_position(player_from, player_to, objects);
    break;
  }
  case index_of<Tile, Door>(): {
    auto door_state =
      is_door_open(std::get<Door>(tile).door_id, doors);
    if (door_state == std::nullopt)
      return std::optional<StepResult>(StepResult::MissingDoorId);
    if (door_state.value() == false)
      return std::optional<StepResult>(StepResult::SlamOnDoor);
    else
      update_position(player_from, player_to, objects);
    break;
  }
  case index_of<Tile, DirFloor>(): {
    if (is_valid_dir(tile, input))
      update_position(player_from, player_to, objects);
    else
      return std::optional<StepResult>(StepResult::InvalidDirection);
    break;
  }
  case index_of<Tile, Portal>(): {
    break;
  }
  }
  return std::nullopt;
}

// move player and the object in the next tile
static constexpr std::optional<StepResult> move_objects() {}

constexpr StepResult State::step(const Direction &input) noexcept {
  auto player_ = find_player(this->objects);
  if (!player_)
    return StepResult::MissingPlayer;
  auto [player_from, player] = player_.value();

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
  if (to_object_ == std::nullopt)
    res = move_player(
      input,
      player_from,
      player_to,
      to_tile,
      this->objects,
      this->doors
    );
  else
    res = move_objects();

  if (res == std::nullopt) {
    // on_exit();
    return StepResult::Ok;
  } else
    return res.value();
}

} // namespace sbokena::game::state
