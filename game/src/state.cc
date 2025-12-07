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

  return iter->first;
}

static constexpr std::optional<Tile> find_tile(
  const std::map<Position<>, Tile> &tiles, const Position<> pos
) {
  auto tile_iter = tiles.find(pos);
  if (tile_iter == tiles.end())
    return std::nullopt;
  auto &[_, tile] = *tile_iter;
  return std::optional<Tile>(tile);
}

static constexpr std::optional<Object> find_object(
  std::map<Position<>, Object> &objects, const Position<> &pos
) {
  auto obj_iter = objects.find(pos);
  if (obj_iter == objects.end())
    return std::nullopt;
  auto &[_, obj] = *obj_iter;
  return std::optional<Object>(obj);
}

// update the position of object in the map
static void update_position(
  const Position<>              from,
  const Position<>              to,
  std::map<Position<>, Object> &objects
) {
  assert_throw(
    objects.contains(from), std::logic_error {"position not exist"}
  );
  auto handler  = objects.extract(from);
  handler.key() = to;
  objects.insert(std::move(handler));
}

// check if a door is opened, aka. one of its corresponding button is
// being pressed. return nullopt if no door of id is found.
static bool is_door_open(
  const u32                               id,
  const std::unordered_map<u32, DoorSet> &doors,
  // FIXME: should be const but currently not const
  std::map<Position<>, Object> &objects
) {
  const auto &[_, buttons] = doors.at(id);
  return std::any_of(
    buttons.begin(), buttons.end(), [&](const auto pos) {
      return objects.contains(pos);
    }
  );
}

// check if user steps in to DirFloor/Portal in the correct
// direction.
static bool is_valid_dir(const Tile &tile, const Direction &dir) {
  if (std::holds_alternative<DirFloor>(tile))
    return std::get<DirFloor>(tile).dir == dir;
  if (std::holds_alternative<Portal>(tile))
    return std::get<Portal>(tile).in_dir == dir;
  // if not either DirFloor or Portal, the direction is always true
  return true;
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
  return std::pair(portal_to.move(out_dir), out_dir); // changed
}

static constexpr std::optional<StepResult> move_object(
  const Direction                            dir,
  const std::map<Position<>, Tile>          &tiles,
  std::map<Position<>, Object>              &objects,
  const std::unordered_map<u32, DoorSet>    &doors,
  const std::unordered_map<u32, PortalPair> &portals,
  const Position<>                          &from,
  const Position<>                          &to
) {
  Object self = objects.at(from);

  // on exit event: check if current tile is DirFloor
  Tile from_tile = tiles.at(from);

  // only check when exit DirFloor and not through portal
  if (std::holds_alternative<DirFloor>(from_tile)
      && from.move(dir) == to)
    if (!is_valid_dir(from_tile, dir))
      return StepResult::InvalidDirection;

  // find destination tile
  auto to_tile_ = find_tile(tiles, to);

  // step on wall
  if (!to_tile_)
    return StepResult::HitWall;
  Tile to_tile = to_tile_.value();

  auto to_object_ = find_object(objects, to);
  if (to_object_) {
    Object to_object = to_object_.value();
    // only call move_object on object found if self is player and
    // object is not
    if (std::holds_alternative<Player>(self)
        && !std::holds_alternative<Player>(to_object)) {
      // dirbox must be move in right direction
      if (std::holds_alternative<DirBox>(to_object)
          && !is_valid_dir(std::get<DirBox>(to_object), dir))
        return StepResult::InvalidDirection;

      // return if not okay, else continue to moving self
      if (auto res = move_object(
            dir, tiles, objects, doors, portals, to, to.move(dir)
          ))
        return res;
    } else // self is not player, meaning that self is box, pushing
           // box or player
      if (!std::holds_alternative<Player>(self))
        return StepResult::PushTwoObjects;
      else
        return StepResult::PushYourself;
  }

  switch (to_tile.index()) {
  case index_of<Tile, Floor>():
  case index_of<Tile, Goal>():
  case index_of<Tile, Button>(): {
    update_position(from, to, objects);
    break;
  }
  case index_of<Tile, DirFloor>(): {
    // if not from portal, must check direction
    if (!std::holds_alternative<Portal>(from_tile)) {
      if (!is_valid_dir(from_tile, dir))
        return StepResult::InvalidDirection;
    }
    update_position(from, to, objects);
    break;
  }
  case index_of<Tile, Door>(): {
    if (!is_door_open(
          std::get<Door>(to_tile).door_id, doors, objects
        ))
      return std::optional<StepResult>(StepResult::SlamOnDoor);
    update_position(from, to, objects);
    break;
  }
  case index_of<Tile, Portal>(): {
    if (!is_valid_dir(to_tile, dir))
      return StepResult::InvalidDirection;
    auto [p_exit, out_dir] = get_portal_exit(
      to, std::get<Portal>(to_tile).portal_id, tiles, portals
    );

    if (auto res_port = move_object(
          out_dir, tiles, objects, doors, portals, from, p_exit
        ))
      return res_port;
  }
  }

  return std::nullopt;
}

StepResult State::step(const Direction &input) noexcept {
  Position<> player_from = find_player(this->objects);

  auto res = move_object(
    input,
    this->tiles,
    this->objects,
    this->doors,
    this->portals,
    player_from,
    player_from.move(input)
  );
  // res == nullopt meaning that positions moved successfully
  return res.value_or(StepResult::Ok);
}

} // namespace sbokena::game::state
