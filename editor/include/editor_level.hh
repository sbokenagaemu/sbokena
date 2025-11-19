#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "object.hh"
#include "position.hh"
#include "tile.hh"
#include "types.hh"

using namespace sbokena::position;

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
    return ++max_id;
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
    return ++max_id;
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

  // functions regarding tiles.

  // create a new tile, takes a type and a position as parameters.
  u32 create_tile(TileType type, const Position<> &pos) {
    u32 id;
    // if a tile already exists at the position, cannot create a new tile.
    auto it = positions.find(pos);
    if (it != positions.end()) {
      return 0x00;
    }
    // else create a new tile there.
    id = this->generate_tile_id();
    std::unique_ptr<Tile> tile;
    switch (type) {
    case TileType::Wall:
      tile = std::make_unique<Wall>(id);
      break;
    case TileType::Floor:
      tile = std::make_unique<Floor>(id);
      break;
    case TileType::OneDir:
      tile = std::make_unique<OneDir>(id);
      break;
    case TileType::Goal:
      tile = std::make_unique<Goal>(id);
      break;
    case TileType::Portal:
      tile = std::make_unique<Portal>(id);
      break;
    case TileType::Door:
      tile = std::make_unique<Door>(id);
      break;
    case TileType::Button:
      tile = std::make_unique<Button>(id);
      break;
    }

    positions[pos] = id;
    tiles.map[id] = std::move(tile);
    return id;
  };

  bool remove_tile(u32 id) {
    // check if the tile actually exists.
    Tile *tile = get_tile(id);
    if (!tile)
      return false;
    // can’t remove tiles that have an object on them.
    if (tile->contains_obj()) {
      return false;
    }

    // clean up links.
    switch (tile->get_type()) {
    case TileType::Portal: {
      auto it = linked_portals.find(id);
      if (it != linked_portals.end()) {
        u32 other = it->second;
        linked_portals.erase(it);
        auto it_other = linked_portals.find(other);
        if (it_other != linked_portals.end()) {
          linked_portals.erase(it_other);
        }
        // unlinks the portal objects.
        if (auto *p = dynamic_cast<Portal *>(tile)) {
          p->unlink();
        }
        if (auto *op = dynamic_cast<Portal *>(get_tile(other))) {
          op->unlink();
        }
      }
      break;
    }
    case TileType::Door: {
      auto it = door_to_button.find(id);
      if (it != door_to_button.end()) {
        u32 button = it->second;
        door_to_button.erase(it);
        auto it_btn = button_to_door.find(button);
        if (it_btn != button_to_door.end()) {
          button_to_door.erase(it_btn);
        }
        // unlinks the door object and set it as closed.
        if (auto *d = dynamic_cast<Door *>(tile)) {
          d->unlink();
          d->close();
        }
        // unlinks the button object.
        if (auto *b = dynamic_cast<Button *>(get_tile(button))) {
          b->unlink();
        }
      }
      break;
    }
    case TileType::Button: {
      auto it = button_to_door.find(id);
      if (it != button_to_door.end()) {
        u32 door = it->second;
        button_to_door.erase(it);
        auto it_door = door_to_button.find(door);
        if (it_door != door_to_button.end()) {
          door_to_button.erase(it_door);
        }
        // unlinks the button object.
        if (auto *b = dynamic_cast<Button *>(tile)) {
          b->unlink();
        }
        // unlinks the door object and set it as closed.
        if (auto *d = dynamic_cast<Door *>(get_tile(door))) {
          d->unlink();
          d->close();
        }
      }
      break;
    }
    default:
      break;
    }

    // remove from positions (position -> id).
    for (auto it = positions.begin(); it != positions.end(); ++it) {
      if (it->second == id) {
        positions.erase(it);
        break;
      }
    }
    // remove from tile map.
    tiles.map.erase(id);
    return true;
  }

  // remove the tile at the specified position.
  bool remove_tile_at(const Position<> &pos) {
    auto it = positions.find(pos);
    // if the position contains no tile then cant remove tile.
    if (it == positions.end())
      return false;
    // else remove tile normally.
    return remove_tile(it->second);
  }

  // the pointer to the tile.
  Tile *get_tile(u32 id) {
    auto it = tiles.map.find(id);
    // if id doesn't exist return nullptr.
    if (it == tiles.map.end())
      return nullptr;
    // else return the tile pointer.
    return it->second.get();
  }

  // the const pointer to the tile.
  const Tile *get_tile(u32 id) const {
    auto it = tiles.map.find(id);
    // if id doesn't exist return nullptr.
    if (it == tiles.map.end())
      return nullptr;
    // else return the tile pointer.
    return it->second.get();
  };

  // returns the pointer to the tile at specified position.
  Tile *get_tile_at(const Position<> &pos) {
    auto it = positions.find(pos);
    // if the position contains no tile then returns nullptr.
    if (it == positions.end())
      return nullptr;
    // else return the tile pointer.
    return get_tile(it->second);
  }

  // returns the const pointer to the tile at specified position.
  const Tile *get_tile_at(const Position<> &pos) const {
    auto it = positions.find(pos);
    // if the position contains no tile then returns nullptr.
    if (it == positions.end())
      return nullptr;
    // else return the tile pointer.
    return get_tile(it->second);
  }

  // whether the position contains the tile.
  bool has_tile_at(const Position<> &pos) const {
    return positions.find(pos) != positions.end();
  }

  // functions regrading objects.

  // adds an object onto an existing tile.
  u32 add_object(ObjectType type, const Position<> &pos) {
    // if the position contains no tile then can't add object.
    if (!has_tile_at(pos))
      return 0x00;
    Tile *tile = get_tile_at(pos);
    // if the tile is a wall then can't add object.
    if (tile->get_type() == TileType::Wall)
      return 0x00;
    // if the tile already contains an object then can't add object.
    if (tile->contains_obj())
      return 0x00;
    // else add an object.
    u32 id = this->generate_object_id();
    std::unique_ptr<Object> object;
    switch (type) {
    case ObjectType::Box:
      object = std::make_unique<Box>(id);
      break;
    case ObjectType::OneDirBox:
      object = std::make_unique<OneDirBox>(id);
      break;
    case ObjectType::Player:
      object = std::make_unique<Player>(id);
      break;
    }
    tile->set_obj_id(id);
    objects.map[id] = std::move(object);
    return id;
  }

  bool remove_object(u32 id) {
    auto it = objects.map.find(id);
    // check if the tile exists.
    if (it == objects.map.end())
      return false;
    // remove the object_id in the tile the object is on.
    for (auto &pair : positions) {
      u32 tile_id = pair.second;
      Tile *tile = get_tile(tile_id);
      if (tile) {
        if (tile->contains_obj() && tile->get_obj_id() == id) {
          tile->remove_obj_id();
          break;
        }
      }
    }
    // remove the object.
    objects.map.erase(it);
    return true;
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
  }

  Object *get_object_at(const Position<> &pos) {
    Tile *tile = get_tile_at(pos);
    if (!tile)
      return nullptr;
    if (!tile->contains_obj())
      return nullptr;
    u32 id = tile->get_obj_id();
    return get_object(id);
  };

  const Object *get_object_at(const Position<> &pos) const {
    const Tile *tile = get_tile_at(pos);
    if (!tile)
      return nullptr;
    if (!tile->contains_obj())
      return nullptr;
    u32 id = tile->get_obj_id();
    return get_object(id);
  }

  bool move_object(u32 id, const Position<> &new_pos) {
    // check if object exists.
    Object *object = get_object(id);
    if (!object)
      return false;
    // check if the tile at the new position exists.
    Tile *tile = get_tile_at(new_pos);
    if (!tile)
      return false;
    // if tile is wall or contains an object, can't move tile there.
    if (tile->get_type() == TileType::Wall || tile->contains_obj())
      return false;
    // remove the object_id in the tile the object is currently on.
    for (auto &pair : positions) {
      u32 tile_id = pair.second;
      Tile *tile = get_tile(tile_id);
      if (tile) {
        if (tile->contains_obj() && tile->get_obj_id() == id) {
          tile->remove_obj_id();
          break;
        }
      }
    }
    // place the object on a new tile.
    tile->set_obj_id(id);
    return true;
  }

  // functions regarding portals.

  // link two portals together.
  bool link_portals(u32 id1, u32 id2) {
    Tile *tile1 = get_tile(id1);
    Tile *tile2 = get_tile(id2);
    // check if the tiles exist.
    if (!tile1 || !tile2)
      return false;
    // check if both tiles are portals.
    if (tile1->get_type() != TileType::Portal ||
        tile2->get_type() != TileType::Portal)
      return false;
    Portal *portal1 = dynamic_cast<Portal *>(tile1);
    Portal *portal2 = dynamic_cast<Portal *>(tile2);
    // both portals must be unlinked in order to link.
    if (portal1->is_linked() || portal2->is_linked())
      return false;
    // link portals.
    portal1->link();
    portal2->link();
    // add the pair in both directions into linked_portals.
    linked_portals[id1] = id2;
    linked_portals[id2] = id1;
    return true;
  }

  bool unlink_portal(u32 portal_id) {
    auto it = linked_portals.find(portal_id);
    // check if the portals pair exists.
    if (it == linked_portals.end())
      return false;
    // the other portal's id.
    u32 other_id = it->second;
    auto it2 = linked_portals.find(other_id);
    // check if the reverse portal pair exists.
    if (it2 == linked_portals.end())
      return false;
    Portal *portal1 = dynamic_cast<Portal *>(get_tile(portal_id));
    Portal *portal2 = dynamic_cast<Portal *>(get_tile(other_id));
    if (!portal1 || !portal2)
      return false;
    // unlink the portals.
    portal1->unlink();
    portal2->unlink();
    // erase the two pairs in linked_portals.
    linked_portals.erase(portal_id);
    linked_portals.erase(other_id);
    return true;
  }

  // functions regarding door-buttons.

  // link a door and a button together.
  bool link_door_button(u32 door_id, u32 button_id) {
    Tile *tile1 = get_tile(door_id);
    Tile *tile2 = get_tile(button_id);
    // check if the tiles exist.
    if (!tile1 || !tile2)
      return false;
    // check if the 1st param is a door and the 2nd param is a button.
    if (tile1->get_type() != TileType::Door ||
        tile2->get_type() != TileType::Button)
      return false;
    Door *door = dynamic_cast<Door *>(tile1);
    Button *button = dynamic_cast<Button *>(tile2);
    if (door->is_linked() || button->is_linked())
      return false;
    // link the door and the button.
    door->link();
    button->link();
    // add the pair in both directions into linked_portals.
    door_to_button[door_id] = button_id;
    button_to_door[button_id] = door_id;
    return true;
  }

  bool unlink_door(u32 door_id);

  bool unlink_button(u32 button_id);

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
  std::unordered_map<Position<>, u32, PositionHash> positions;
  // linked portals, stores both portal 1 -> portal 2 and vice versa.
  std::unordered_map<u32, u32> linked_portals;
  // stores door_id to button_id;
  std::unordered_map<u32, u32> door_to_button;
  // stores button_id to door_id
  std::unordered_map<u32, u32> button_to_door;
};