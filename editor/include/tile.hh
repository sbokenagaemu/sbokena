// A file implementing the tiles in the grid inside the
// editor. The tile contains its absolute/relative position
// within the grid (still contested), the list of objects
// above it (for example, the character or the box above the
// floor), and its sprite image.

#pragma once

#include <algorithm>
#include <vector>

#include "direction.hh"
#include "types.hh"

using sbokena::direction::Direction;
using sbokena::types::u32;

namespace sbokena::editor::tile {

// invalid or empty id, as id indexing starts from 0x01.
static constexpr u32 null_id = 0x00;

// An enum containing types of tiles.
enum class TileType {
  Roof,
  Floor,
  OneDir,
  Goal,
  Portal,
  Door,
  Button,
};

// A class containing common fields and functions that all
// tiles share.
class Tile {
public:
  // construct a new tile, contains no object by default.
  // obj_id = null_id -> no id linked (id indexing starts at 0x01).
  explicit Tile(TileType type, u32 id)
    : type(type),
      id(id),
      obj_id(null_id) {}

  // default destructor.
  virtual ~Tile() = default;

  // returns tile type.
  TileType get_type() const {
    return type;
  }

  // tile's id.
  u32 get_id() const {
    return id;
  }

  // if there is an object on the tile.
  virtual bool contains_obj() const {
    return (obj_id != null_id);
  }

  // tile's object id.
  virtual u32 get_obj_id() const {
    return obj_id;
  }

  // object goes on tile.
  virtual void set_obj_id(const u32 &new_id) {
    obj_id = new_id;
  }

  // object goes off tile.
  virtual void remove_obj_id() {
    obj_id = null_id;
  }

private:
  TileType type;
  u32      id;
  u32      obj_id;
  // TODO: an image related id
};

// The default type for a tile. Does not and cannot contain
// any object.
class Roof : public Tile {
public:
  // construct a new roof.
  explicit Roof(u32 id) : Tile(TileType::Roof, id) {}

  // does not contain object.
  bool contains_obj() const override {
    return false;
  }

  // no object, no id.
  u32 get_obj_id() const override {
    return null_id;
  }

  // object cannot be on roof.
  void set_obj_id(const u32 &) override {}

  // does not have object to remove to begin with.
  void remove_obj_id() override {}
};

// The basic, most common type of non-roof tile. Can contain
// an object.
class Floor : public Tile {
public:
  // construct a new floor.
  Floor(u32 id) : Tile(TileType::Floor, id) {}
};

// Similar to floor but only lets object move into it
// through one specified way. To move out of the tile, the
// object can only move the opposite way.
class OneDir : public Tile {
public:
  // construct a new one direction floor.
  // by default the direction into the tile is up.
  OneDir(u32 id)
    : Tile(TileType::OneDir, id),
      dir_in(Direction::Up) {}

  // which only direction for the object to move into this
  // tile.
  Direction get_dir_in() const {
    return dir_in;
  }

  // which only direction for the object to move out of this
  // tile.
  Direction get_dir_out() const {
    Direction dir_out;
    switch (dir_in) {
    case Direction::Up:
      dir_out = Direction::Down;
      break;
    case Direction::Right:
      dir_out = Direction::Left;
      break;
    case Direction::Down:
      dir_out = Direction::Up;
      break;
    case Direction::Left:
      dir_out = Direction::Right;
      break;
    }
    return dir_out;
  }

  // can only move into and out of this tile by this new
  // direction and its opposite direction respectively.
  void set_dir_in(Direction newdir) {
    dir_in = newdir;
  }

  // rotates clockwise.
  void rotate() {
    switch (dir_in) {
    case Direction::Up:
      dir_in = Direction::Right;
      break;
    case Direction::Right:
      dir_in = Direction::Down;
      break;
    case Direction::Down:
      dir_in = Direction::Left;
      break;
    case Direction::Left:
      dir_in = Direction::Up;
      break;
    }
  }

private:
  Direction dir_in;
};

// If the player arrives into this tile, the level is
// completed.
class Goal : public Tile {
public:
  Goal(u32 id) : Tile(TileType::Goal, id) {}
};

// Can teleport objects to the linked portal.
// Only lets object move in and out of the tile in a
// specific way and its opposite way respectively.
class Portal : public Tile {
public:
  // construct a new portal, by default is unlinked to
  // another portal. by default the direction into the tile
  // is up.
  Portal(u32 id)
    : Tile(TileType::Portal, id),
      portal_id(null_id),
      dir_in(Direction::Up) {}

  // links the portal to another portal.
  void link(u32 linked_id) {
    portal_id = linked_id;
  }

  // unlinks the portal.
  void unlink() {
    portal_id = null_id;
  }

  // whether the portal is linked or not.
  bool is_linked() const {
    return (portal_id != null_id);
  }

  // the linked portal id.
  u32 get_linked() const {
    return portal_id;
  }

  // which only direction for the object to move into this
  // tile.
  Direction get_dir_in() const {
    return dir_in;
  }

  // which only direction for the object to move out of this
  // tile.
  Direction get_dir_out() const {
    Direction dir_out;
    switch (dir_in) {
    case Direction::Up:
      dir_out = Direction::Down;
      break;
    case Direction::Right:
      dir_out = Direction::Left;
      break;
    case Direction::Down:
      dir_out = Direction::Up;
      break;
    case Direction::Left:
      dir_out = Direction::Right;
      break;
    }
    return dir_out;
  }

  // can only move into and out of this tile by this new
  // direction and its opposite direction respectively.
  void set_dir_in(Direction newdir) {
    dir_in = newdir;
  }

  // rotates clockwise.
  void rotate() {
    switch (dir_in) {
    case Direction::Up:
      dir_in = Direction::Right;
      break;
    case Direction::Right:
      dir_in = Direction::Down;
      break;
    case Direction::Down:
      dir_in = Direction::Left;
      break;
    case Direction::Left:
      dir_in = Direction::Up;
      break;
    }
  }

private:
  u32       portal_id;
  Direction dir_in;
};

// Is linked to one or more buttons. When all are pressed, tries to
// open the door. If contains an object, the state doesn't change
// untill it stops containing one.
class Door : public Tile {
public:
  // constructs a new door, by default isn't linked to another button
  // and isn't opened.
  Door(u32 id)
    : Tile(TileType::Door, id),
      button_ids(),
      opened(false) {}

  // links the door to a button; prevents duplicates.
  void link(u32 linked_id) {
    auto it =
      std::find(button_ids.begin(), button_ids.end(), linked_id);
    if (it == button_ids.end())
      button_ids.push_back(linked_id);
  }

  // unlinks the door from a button.
  void unlink(u32 unlinked_id) {
    auto it =
      std::find(button_ids.begin(), button_ids.end(), unlinked_id);
    if (it != button_ids.end())
      button_ids.erase(it);
  }

  // unlinks the door from all buttons.
  void unlink_all() {
    button_ids.clear();
  }

  // whether the door is linked or not.
  bool is_linked() const {
    return !button_ids.empty();
  }

  // the vector of linked ids.
  std::vector<u32> &get_linked_vector() {
    return button_ids;
  }

  const std::vector<u32> &get_linked_vector() const {
    return button_ids;
  }

  // opens the door.
  // if there is an object on the door, can't change the
  // door's state.
  bool open() {
    if (contains_obj())
      return false;
    opened = true;
    return true;
  }

  // closes the door.
  // if there is an object on the door, can't change the
  // door's state.
  bool close() {
    if (contains_obj())
      return false;
    opened = false;
    return true;
  }

  // is the door opened or closed.
  bool is_opened() const {
    return opened;
  }

private:
  std::vector<u32> button_ids;
  bool             opened;
};

// Is linked to a door, tries to it when contains an object (pressed).
class Button : public Tile {
public:
  // constructs a new button, by default isn't linked to another door.
  Button(u32 id) : Tile(TileType::Button, id), door_id(null_id) {}

  // links the button to a door.
  void link(u32 linked_id) {
    door_id = linked_id;
  }

  // unlinks the button.
  void unlink() {
    door_id = null_id;
  }

  // whether the button is linked or not.
  bool is_linked() const {
    return (door_id != null_id);
  }

  // the linked door id.
  u32 get_linked() const {
    return door_id;
  }

private:
  u32 door_id;
};

} // namespace sbokena::editor::tile
