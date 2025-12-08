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
find_player(const std::map<Position<>, Object> &objects) {
  return std::find_if(
           objects.begin(), objects.end(), [](const auto &p) {
             return std::holds_alternative<Player>(p.second);
           }
  )->first;
}

// given position, return tile at that position.
static constexpr std::optional<Tile> find_tile(
  const std::map<Position<>, Tile> &tiles, const Position<> pos
) {
  auto tile_iter = tiles.find(pos);
  if (tile_iter == tiles.end())
    return std::nullopt;
  return {tile_iter->second};
}

// given position, return object at that position.
static constexpr std::optional<Object> find_object(
  const std::map<Position<>, Object> &objects, const Position<> &pos
) {
  auto obj_iter = objects.find(pos);
  if (obj_iter == objects.end())
    return std::nullopt;
  return {obj_iter->second};
}

// update the position of object in the map.
static void update_position(
  const Position<>              from,
  const Position<>              to,
  std::map<Position<>, Object> &objects
) {
  assert_throw(
    objects.contains(from),
    std::logic_error {"no object at this position"}
  );
  auto handle  = objects.extract(from);
  handle.key() = to;
  objects.insert(std::move(handle));
}

// check if a door is opened, aka. one of its corresponding button is
// being pressed.
static bool is_door_open(
  const u32                               id,
  const std::unordered_map<u32, DoorSet> &doors,
  const std::map<Position<>, Object>     &objects
) {
  const auto &[_, buttons] = doors.at(id);
  return std::any_of(
    buttons.begin(), buttons.end(), [&](const auto &pos) {
      return objects.contains(pos);
    }
  );
}

// check if user moves in the correct direction.
static bool is_valid_dir(const Tile &tile, const Direction &dir) {
  if (std::holds_alternative<DirFloor>(tile))
    return std::get<DirFloor>(tile).dir == dir;
  if (std::holds_alternative<Portal>(tile))
    return std::get<Portal>(tile).in_dir == dir;
  // if not either DirFloor or Portal, the direction is always true.
  return true;
}

// check if DirBox is pushed in correct direction.
static bool is_valid_dir(const DirBox &box, const Direction &step) {
  return box.dir == step;
}

// return both exit position from Portal and direction of exit.
static std::pair<Position<>, Direction> get_portal_exit(
  const Position<>                          &portal_from,
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
  return {portal_to.move(out_dir), out_dir};
}

// update object's position if possible. else return the error.
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

  // on exit event: check if current tile is DirFloor.
  Tile from_tile = tiles.at(from);

  // check validity of exit.
  if (!is_valid_dir(from_tile, dir))
    return StepResult::InvalidDirection;

  // find destination tile.
  auto to_tile_ = find_tile(tiles, to);

  // hit wall.
  if (!to_tile_)
    return StepResult::HitWall;
  Tile to_tile = to_tile_.value();

  auto to_object_ = find_object(objects, to);
  if (to_object_) {
    Object to_object = to_object_.value();
    // only call move_object when self is player and object is not.
    if (std::holds_alternative<Player>(self)
        && !std::holds_alternative<Player>(to_object)) {
      // DirBox must be move in right direction.
      if (std::holds_alternative<DirBox>(to_object)
          && !is_valid_dir(std::get<DirBox>(to_object), dir))
        return StepResult::InvalidDirection;

      // return if not okay, else continue to moving self.
      auto res = move_object(
        dir, tiles, objects, doors, portals, to, to.move(dir)
      );
      if (res)
        return res;
    } else
      // self is Box, cannot push anything.
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
    // if not from Portal, must check direction.
    if (from.move(dir) == to)
      if (!is_valid_dir(from_tile, dir))
        return StepResult::InvalidDirection;
    update_position(from, to, objects);
    break;
  }
  case index_of<Tile, Door>(): {
    if (!is_door_open(
          std::get<Door>(to_tile).door_id, doors, objects
        ))
      return {StepResult::SlamOnDoor};
    update_position(from, to, objects);
    break;
  }
  case index_of<Tile, Portal>(): {
    // always check direction when enter Portal.
    if (!is_valid_dir(to_tile, dir))
      return StepResult::InvalidDirection;
    auto [p_exit, out_dir] = get_portal_exit(
      to, std::get<Portal>(to_tile).portal_id, tiles, portals
    );

    // call move_object on position exiting Portal.
    auto res_port = move_object(
      out_dir, tiles, objects, doors, portals, from, p_exit
    );
    if (res_port)
      return res_port;
  }
  }
  return std::nullopt;
}

// query current points.
usize points_query(
  const std::vector<Position<>>      &goals,
  const std::map<Position<>, Object> &objects
) {
  return std::ranges::count_if(
    goals.begin(), goals.end(), [objects](Position<> p) {
      auto r = (find_object(objects, p));
      if (r)
        return !std::holds_alternative<Player>(r.value());
      return false;
    }
  );
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
  // return res if there is any invalid move.
  if (res)
    return res.value();
  else if (this->goal.size() > 0
           && points_query(this->goal, this->objects) == goal.size())
    return StepResult::LevelComplete;
  else
    return StepResult::Ok;
}

} // namespace sbokena::game::state
