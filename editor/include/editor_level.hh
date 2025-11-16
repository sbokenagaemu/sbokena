#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "object.hh"
#include "position.hh"
#include "tile.hh"
#include "types.hh"

enum class Difficulty { Unknown, Easy, Medium, Hard };

// contains the difficulty and move limits of the game.
struct Condition {
  // constructor
  // by default set difficulty and move limit to unknown and 0 respectively.
  Condition() : difficulty(Difficulty::Unknown), move_limit(0) {}

  void set_difficulty(Difficulty diff) {
    difficulty = diff;
  }

  // set difficulty back to unknown.
  void clear_difficulty() {
    difficulty = Difficulty::Unknown;
  }

  void set_move_limit(u32 lim) {
    move_limit = lim;
  }

  void clear_move_limit() {
    move_limit = 0;
  }

  Difficulty difficulty;
  u32 move_limit;
};

// an unordered map using tile ids to access tiles.
struct TileMap {
  // initialize an empty map and no ids (id starts from 0x01).
  TileMap() : map(), max_id(0x00) {}
  ~TileMap() = default;

  // return a new id, which is the id with the max value incremented by 1.
  u32 generate_new_id() {
    return max_id++;
  }

  std::unordered_map<u32, std::unique_ptr<Tile>> map;
  u32 max_id;
};

// an unordered map using object ids to access objects.
struct ObjectMap {
  // initialize an empty map and no ids (id starts from 0x01).
  ObjectMap() : map(), max_id(0x00) {}
  ~ObjectMap() = default;

  // return a new id, which is the id with the max value incremented by 1.
  u32 generate_new_id() {
    return max_id++;
  }

  std::unordered_map<u32, std::unique_ptr<Object>> map;
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

// contains the positions of tiles, uses positions to access tile ids.
struct PositionMap {
  PositionMap() : map() {}
  std::unordered_map<Position<>, u32, PositionHash> map;
};

// Contains and manages all tiles and objects.
class Level {
public:
  // constructor; apart from names, all other fields are defaults.
  // clang-format off
  explicit Level(std::string &name) : 
    name(name), 
    condition(), 
    tiles(),
    objects(), 
    positions() {}
  // clang-format on

  ~Level() = default;

  const std::string &get_name() const noexcept {
    return name;
  }

  void change_name(const std::string &str) {
    name = str;
  }

  const Condition &get_condition() const noexcept {
    return condition;
  }

  Condition &get_condition() noexcept {
    return condition;
  }

  u32 generate_tile_id() {
    return tiles.generate_new_id();
  }

  u32 generate_object_id() {
    return objects.generate_new_id();
  }

  u32 create_tile(TileType type, const Position<> &pos) {
    u32 id;
    // if a tile already exists at the position, replace it
    auto it = positions.map.find(pos);
    if (it != positions.map.end()) {
      id = it->second;
      tiles.map.erase(it->second);
      positions.map.erase(it);
    }
    // else, create a new tile there
    id = this->generate_tile_id();
    std::unique_ptr<Tile> tile;
    switch (type) {
    case TileType::Wall:
      tile = std::make_unique<Wall>(id, pos);
      break;
    case TileType::Floor:
      tile = std::make_unique<Floor>(id, pos);
      break;
    case TileType::Goal:
      tile = std::make_unique<Goal>(id, pos);
      break;
    case TileType::Portal:
      tile = std::make_unique<Portal>(id, pos);
      break;
    case TileType::Door:
      tile = std::make_unique<Door>(id, pos);
      break;
    case TileType::Button:
      tile = std::make_unique<Button>(id, pos);
      break;
    case TileType::OneDir:
      tile = std::make_unique<OneDir>(id, pos);
      break;
    }

    positions.map[pos] = id;
    tiles.map[id] = std::move(tile);
    return id;
  };

  bool remove_tile(u32 tile_id);

  u32 create_object(ObjectType type) {
    u32 id = objects.generate_new_id();

    std::unique_ptr<Object> obj;
    switch (type) {
    case ObjectType::Player:
      obj = std::make_unique<Player>(id);
      break;
    case ObjectType::Box:
      obj = std::make_unique<Box>(id);
      break;
    case ObjectType::OneDirBox:
      obj = std::make_unique<OneDirBox>(id);
      break;
    }

    objects.map[id] = std::move(obj);
    return id;
  }

  bool remove_object(u32 object_id) {

  };

  Tile *get_tile(u32 id) {
    auto it = tiles.map.find(id);
    if (it == tiles.map.end())
      return nullptr;
    return it->second.get();
  }

  const Tile *get_tile(u32 id) const {
    auto it = tiles.map.find(id);
    if (it == tiles.map.end())
      return nullptr;
    return it->second.get();
  };

  Object *get_object(u32 id) {
    auto it = objects.map.find(id);
    if (it == objects.map.end())
      return nullptr;
    return it->second.get();
  };

  const Object *get_object(u32 id) const {
    auto it = objects.map.find(id);
    if (it == objects.map.end())
      return nullptr;
    return it->second.get();
  };

  Tile *get_tile_at(const Position<> &pos);

  const Tile *get_tile_at(const Position<> &pos) const;

  bool has_tile_at(const Position<> &pos) const;

  bool change_tile_type_at(const Position<> &pos, TileType new_type);

  bool place_object_at(const Position<> &pos, u32 object_id);

  bool remove_object_at(const Position<> &pos);

  bool move_object(const Position<> &from, const Position<> &to);

  // TODO: functions related to image resources

private:
  std::string name;
  Condition condition;

  // TODO: a grid stucture storing the skins of every position.
  // map<position, image-related-id>

  // id -> tile
  TileMap tiles;
  // id -> object
  ObjectMap objects;
  // position -> tile_id
  PositionMap positions;
};
