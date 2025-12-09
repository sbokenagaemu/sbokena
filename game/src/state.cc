#include "state.hh"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <variant>

#include "level.hh"
#include "utils.hh"

using namespace sbokena::utils;

namespace sbokena::game::state {

Position<> State::find_player() const {
  const auto iter =
    std::find_if(objects.begin(), objects.end(), [](const auto &p) {
      return std::holds_alternative<Player>(p.second);
    });
  return iter->first;
}

std::optional<Object> State::find_object(const Position<> pos) const {
  const auto iter = objects.find(pos);
  if (iter == objects.end())
    return std::nullopt;
  return {iter->second};
}

std::optional<Tile> State::find_tile(const Position<> pos) const {
  const auto iter = tiles.find(pos);
  if (iter == tiles.end())
    return std::nullopt;
  return {iter->second};
}

std::pair<Position<>, Direction>
State::get_portal_exit(Position<> portal_from, u32 id) const {
  auto portal_iter = portals.find(id);
  assert_throw(
    portal_iter != portals.end(),
    std::logic_error {"portal not found"}
  );
  const auto      portal_pair = portal_iter->second;
  const auto      portal_to   = portal_from == portal_pair.first
                                ? portal_pair.second
                                : portal_pair.first;
  const Tile      portal_to_  = find_tile(portal_to).value();
  const Direction out_dir     = -std::get<Portal>(portal_to_).in_dir;
  return {portal_to.move(out_dir), out_dir};
}

usize State::points_query() const {
  return std::ranges::count_if(
    goals.begin(), goals.end(), [&](Position<> p) {
      auto r = (find_object(p));
      if (r)
        return !std::holds_alternative<Player>(r.value());
      return false;
    }
  );
}

bool State::is_door_open(const u32 id) const {
  const auto &[_, buttons] = doors.at(id);
  return std::any_of(
    buttons.begin(), buttons.end(), [&](const auto &pos) {
      return objects.contains(pos);
    }
  );
}

bool State::is_valid_dir(const Tile &tile, Direction dir) const {
  if (std::holds_alternative<DirFloor>(tile))
    return std::get<DirFloor>(tile).dir == dir;
  if (std::holds_alternative<Portal>(tile))
    return std::get<Portal>(tile).in_dir == dir;
  // if not either DirFloor or Portal, the direction is always true.
  return true;
}

bool State::is_valid_dir(const Object &box, Direction step) const {
  if (std::holds_alternative<DirBox>(box))
    return std::get<DirBox>(box).dir == step;
  return true;
}

void State::update_position(
  const Position<> from, const Position<> to
) {
  assert_throw(
    objects.contains(from),
    std::logic_error {"no object at this position"}
  );
  auto handle  = objects.extract(from);
  handle.key() = to;
  objects.insert(std::move(handle));
}

std::optional<StepResult>
State::move_object(Direction dir, Position<> from, Position<> to) {
  const Object self = objects.at(from);

  // on exit event: check if current tile is DirFloor.
  const Tile from_tile = tiles.at(from);

  // check validity of exit.
  if (!is_valid_dir(from_tile, dir))
    return StepResult::InvalidDirection;

  // find destination tile.
  const auto to_tile_ = find_tile(to);

  // hit wall.
  if (!to_tile_)
    return StepResult::HitWall;
  const Tile to_tile = to_tile_.value();

  const auto to_object_ = find_object(to);
  if (to_object_) {
    Object to_object = to_object_.value();
    // only call move_object when self is player and object is not.
    if (std::holds_alternative<Player>(self)
        && !std::holds_alternative<Player>(to_object)) {
      // check moving direction of object
      if (!is_valid_dir(to_object, dir))
        return StepResult::InvalidDirection;

      // return if not okay, else continue to moving self.
      const auto res = move_object(dir, to, to.move(dir));
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
  case index_of<Tile, Button>():
    update_position(from, to);
    break;
  case index_of<Tile, DirFloor>():
    // if not from Portal, must check direction.
    if (from.move(dir) == to)
      if (!is_valid_dir(to_tile, dir))
        return StepResult::InvalidDirection;
    update_position(from, to);
    break;
  case index_of<Tile, Door>(): {
    const Door &door = std::get<Door>(to_tile);
    if (!is_door_open(door.door_id))
      return {StepResult::SlamOnDoor};
    update_position(from, to);
    break;
  }
  case index_of<Tile, Portal>(): {
    // always check direction when enter Portal.
    if (!is_valid_dir(to_tile, dir))
      return StepResult::InvalidDirection;
    const auto [p_exit, out_dir] =
      get_portal_exit(to, std::get<Portal>(to_tile).portal_id);

    // call move_object on position exiting Portal.
    const auto res_port = move_object(out_dir, from, p_exit);
    if (res_port)
      return res_port;
  }
  }
  return std::nullopt;
}

StepResult State::step(const Direction &input) {
  Position<> player_from = find_player();

  const auto res =
    move_object(input, player_from, player_from.move(input));
  // return res if there is any invalid move.
  if (res)
    return res.value();
  else if (goals.size() > 0 && points_query() == goals.size())
    return StepResult::LevelComplete;
  else
    return StepResult::Ok;
}

} // namespace sbokena::game::state
