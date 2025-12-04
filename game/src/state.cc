#include "state.hh"

#include <algorithm>
#include <optional>
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
find_tile(std::map<Position<>, Tile> &tiles, Position<> &pos) {
  auto tile_iter = tiles.find(pos);
  if (tile_iter == tiles.end())
    return std::nullopt;
  auto &[_, tile] = *tile_iter;
  return std::optional<Tile>(tile);
}

static constexpr std::optional<Object>
find_object(std::map<Position<>, Object> &objects, Position<> &pos) {
  auto obj_iter = objects.find(pos);
  if (obj_iter == objects.end())
    return std::nullopt;
  auto &[_, obj] = *obj_iter;
  return std::optional<Object>(obj);
}

// update an object's position in the map
void update_position(
  Position<>                    from,
  Position<>                    to,
  std::map<Position<>, Object> &objects
) {
  auto nodeHandler  = objects.extract(from);
  nodeHandler.key() = to;
  objects.insert(std::move(nodeHandler));
}

std::optional<bool>
is_door_open(u32 id, std::unordered_map<u32, DoorSet> &doors) {
  auto door_iter = doors.find(id);
  if (door_iter == doors.end())
    return std::nullopt;
  auto buttons = door_iter->second.second;
  for (auto button : buttons) {}
  return std::optional<bool>(false);
}

bool is_valid_dir(Tile tile, Direction step) {
  return std::get<DirFloor>(tile).dir == step;
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
