// A file containing the implementation of a level inside an editor along with
// all of its need structures. The level contains:
//  - A Condition struct denoting its difficulty and move limits.
//  - A TileMap struct storing every tile's id mapped to its unique_ptr.
//  - An ObjectMap struct storing every object's id mapped to its unique_ptr.
//  - A PositionMap struct storing every initialized position mapped to its
//  tile_id, and a PositionHash for providing its hash function.
//  - An unordered map for storing all pairs of linked portals.
//  - An unordered map for storing all pairs of linked button to door.
//  - An unordered map for storing all pairs of linked door to button.
// Note: id indexing starts at 0x01, therefore 0x00 means a null or invalid id.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "object.hh"
#include "position.hh"
#include "tile.hh"
#include "types.hh"

using namespace sbokena::position;
using namespace sbokena::editor::tile;
using namespace sbokena::editor::object;

namespace sbokena::editor::editor_level {

// enum for the difficulty.
enum class Difficulty { Unknown, Easy, Medium, Hard };

// invalid or empty id, as id indexing starts from 0x01.
static constexpr u32 null_id = 0x00;

// contains the difficulty and, move limit, and time limit of the game.
struct Condition {
  // constructor
  // by default: difficulty is unknown, both move_limit and time_limit are 0.
  Condition() : difficulty(Difficulty::Unknown), move_limit(0), time_limit(0) {}
  // sets difficulty.
  void set_difficulty(Difficulty diff) {
    difficulty = diff;
  }
  // difficulty in string form.
  std::string diff_to_string() {
    switch (difficulty) {
    case Difficulty::Easy:
      return "easy";
    case Difficulty::Medium:
      return "medium";
    case Difficulty::Hard:
      return "hard";
    default:
      return "unknown";
    }
  }
  // sets difficulty back to unknown.
  void clear_difficulty() {
    difficulty = Difficulty::Unknown;
  }
  // sets move limit.
  void set_move_limit(u32 lim) {
    move_limit = lim;
  }
  // removes move limit.
  void clear_move_limit() {
    move_limit = 0;
  }
  // sets time limit in seconds.
  void set_time_limit(u32 seconds) {
    time_limit = seconds;
  }
  // removes time limit.
  void clear_time_limit() {
    time_limit = 0;
  }
  // fields.
  Difficulty difficulty;
  u32 move_limit;
  u32 time_limit; // in seconds.
};

// an unordered map using tile ids to access tiles.
struct TileMap {
  // initialize an empty map and no ids (id starts from 0x01).
  TileMap() : map(), max_id(null_id) {}
  ~TileMap() = default;
  // return a new id, which is the id with the max value incremented by 1.
  u32 generate_new_id() {
    return ++max_id;
  }
  // the container itself.
  std::unordered_map<u32, std::unique_ptr<Tile>> map;
  // used for generating ids.
  u32 max_id;
};

// an unordered map using object ids to access objects.
struct ObjectMap {
  // initialize an empty map and no ids (id starts from 0x01).
  ObjectMap() : map(), max_id(null_id) {}
  ~ObjectMap() = default;
  // return a new id, which is the id with the max value incremented by 1.
  u32 generate_new_id() {
    return ++max_id;
  }
  // the container itself.
  std::unordered_map<u32, std::unique_ptr<Object>> map;
  // used for generating ids.
  u32 max_id;
};

// hashes positions to use for an unordered map.
struct PositionHash {
  usize operator()(const Position<u32> &pos) const noexcept {
    std::size_t h1 = std::hash<uint32_t>{}(pos.x);
    std::size_t h2 = std::hash<uint32_t>{}(pos.y);
    return h1 ^ (h2 << 1);
  }
};

// Contains and manages all tiles and objects.
class Level {
public:
  // constructor; apart from names, all other fields are defaults.
  // clang-format off
  explicit Level(const std::string &name) : 
    name(name), 
    condition(), 
    tiles(),
    objects(), 
    positions() {}
  // clang-format on

  ~Level() = default;

  // returns level name.
  const std::string &get_name() const noexcept {
    return name;
  }

  // changes level name.
  void change_name(const std::string &str) {
    name = str;
  }

  // returns level condition.
  const Condition &get_condition() const noexcept {
    return condition;
  }

  // changes level condition.
  Condition &get_condition() noexcept {
    return condition;
  }

  // generates a new id for a new tile.
  u32 generate_tile_id() {
    return tiles.generate_new_id();
  }

  // generates a new id for a new object.
  u32 generate_object_id() {
    return objects.generate_new_id();
  }

  // Functions regarding tiles.

  // creates a new tile, takes a type and a position as parameters.
  u32 create_tile(TileType type, const Position<> &pos);

  // removes a tile that doesn't have an object on.
  bool remove_tile(u32 id);

  // removes the tile at the specified position.
  bool remove_tile_at(const Position<> &pos);

  // the pointer to the tile.
  Tile *get_tile(u32 id);

  // the const pointer to the tile.
  const Tile *get_tile(u32 id) const;

  // returns the pointer to the tile at the specified position.
  Tile *get_tile_at(const Position<> &pos);

  // returns the const pointer to the tile at the specified position.
  const Tile *get_tile_at(const Position<> &pos) const;

  // whether the position contains the tile.
  bool has_tile_at(const Position<> &pos) const {
    return positions.find(pos) != positions.end();
  }

  // Functions regrading objects.

  // adds an object onto an existing tile.
  u32 add_object(ObjectType type, const Position<> &pos);

  // removes an object.
  bool remove_object(u32 id);

  // the pointer to the object.
  Object *get_object(u32 id);

  // the const pointer to the object.
  const Object *get_object(u32 id) const;

  // returns the pointer to the object at the specified position.
  Object *get_object_at(const Position<> &pos);

  // returns the const pointer to the object at the specified position.
  const Object *get_object_at(const Position<> &pos) const;

  // moves the object to another existing position.
  bool move_object(u32 id, const Position<> &new_pos);

  // Functions regarding portal pairs.

  // links two portals together.
  bool link_portals(u32 id1, u32 id2);

  // unlinks a portal, which also unlinks the other portal.
  bool unlink_portal(u32 portal_id);

  // Functions regarding door-button pairs.

  // links a door and a button together.
  bool link_door_button(u32 door_id, u32 button_id);

  // unlinks the door, which also unlinks the button.
  bool unlink_door(u32 door_id);

  // unlinks the button, which also unlinks the door.
  bool unlink_button(u32 button_id);

private:
  // the level name.
  std::string name;
  // level difficulty, move limit, and time limit.
  Condition condition;
  // id -> tile.
  TileMap tiles;
  // id -> object.
  ObjectMap objects;
  // position -> tile_id.
  std::unordered_map<Position<>, u32, PositionHash> positions;
  // linked portals, stores both portal 1 -> portal 2 and vice versa.
  std::unordered_map<u32, u32> linked_portals;
  // stores door_id to button_id.
  std::unordered_map<u32, u32> door_to_button;
  // stores button_id to door_id.
  std::unordered_map<u32, u32> button_to_door;
  // TODO: a grid stucture storing the skins of every position.
  // map<position, image-related-id>
};

} // namespace sbokena::editor::editor_level