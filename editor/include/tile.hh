// A file implementing the tiles in the grid inside the editor. The tile
// contains its absolute/relative position within the grid (still contested),
// the list of objects above it (for example, the character or the box above the
// floor), and its sprite image.

#pragma once

#include "direction.hh"
#include "types.hh"

using namespace sbokena::types;
using namespace sbokena::direction;

namespace sbokena::editor::tile {

// An enum containing types of tiles.
enum class TileType {
  Wall,
  Floor,
  OneDir,
  Goal,
  Portal,
  Door,
  Button,
};

// A class containing common fields and functions that all tiles share.
class Tile {
public:
  // construct a new tile, contains no object by default.
  // obj_id = 0x00 -> no id linked (id indexing starts at 0x01).
  explicit Tile(TileType type, u32 id) : type(type), id(id), obj_id(0x00) {}

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
    return (obj_id != 0x00);
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
    obj_id = 0x00;
  }

private:
  TileType type;
  u32 id;
  u32 obj_id;
  // TODO: an image related id
};

// The default type for a tile. Does not and cannot contain any object.
class Wall : public Tile {
public:
  // construct a new wall.
  explicit Wall(u32 id) : Tile(TileType::Wall, id) {}

  // does not contain object.
  bool contains_obj() const override {
    return false;
  }

  // no object, no id.
  u32 get_obj_id() const override {
    return 0x00;
  }

  // object cannot be on wall.
  void set_obj_id(const u32 &) override {}

  // does not have object to remove to begin with.
  void remove_obj_id() override {}
};

// The basic, most common type of non-wall tile. Can contain an object.
class Floor : public Tile {
public:
  // construct a new floor.
  Floor(u32 id) : Tile(TileType::Floor, id) {}
};

// Similar to floor but only lets object move into it through one specified way.
// To move out of the tile, the object can only move the opposite way.
class OneDir : public Tile {
public:
  // construct a new one direction floor.
  // by default the direction into the tile is up.
  OneDir(u32 id) : Tile(TileType::OneDir, id), dir_in(Direction::Up) {}

  // which only direction for the object to move into this tile.
  Direction get_dir_in() const {
    return dir_in;
  }

  // which only direction for the object to move out of this tile.
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

  // can only move into and out of this tile by this new direction and its
  // opposite direction respectively.
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

// If the player arrives into this tile, the level is completed.
class Goal : public Tile {
public:
  Goal(u32 id) : Tile(TileType::Goal, id) {}
};

// Can teleport objects to the linked portal.
// Only lets object move in and out of the tile in a specific way and its
// opposite way respectively.
class Portal : public Tile {
public:
  // construct a new portal, by default is unlinked to another portal.
  // by default the direction into the tile is up.
  Portal(u32 id)
    : Tile(TileType::Portal, id), linked(false), dir_in(Direction::Up) {}

  // flags the portal as linked.
  void link() {
    linked = true;
  }

  // flags the portal as unlinked.
  void unlink() {
    linked = false;
  }

  // whether the portal is linked or not.
  bool is_linked() const {
    return linked;
  }

  // which only direction for the object to move into this tile.
  Direction get_dir_in() const {
    return dir_in;
  }

  // which only direction for the object to move out of this tile.
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

  // can only move into and out of this tile by this new direction and its
  // opposite direction respectively.
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
  bool linked;
  Direction dir_in;
};

// Opens when the linked button is pressed.
class Door : public Tile {
public:
  // construct a new door, by default isn't linked to another button and isn't
  // opened.
  Door(u32 id) : Tile(TileType::Door, id), linked(false), opened(false) {}

  // flags the door as linked.
  void link() {
    linked = true;
  }

  // flags the door as unlinked.
  void unlink() {
    linked = false;
  }

  // whether the door is linked or not.
  bool is_linked() const {
    return linked;
  }

  // opens the door.
  // if there is an object on the door, can't change the door's state.
  bool open() {
    if (contains_obj())
      return false;
    opened = true;
    return true;
  }

  // closes the door.
  // if there is an object on the door, can't change the door's state.
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
  bool linked;
  bool opened;
};

// When contains an object (pressed), opens the linked door.
class Button : public Tile {
public:
  // construct a new button, by default isn't linked to another door.
  Button(u32 id) : Tile(TileType::Button, id), linked(false) {}

  // flags the button as linked.
  void link() {
    linked = true;
  }

  // flags the button as unlinked.
  void unlink() {
    linked = false;
  }

  // whether the button is linked or not.
  bool is_linked() const {
    return linked;
  }

private:
  bool linked;
};

} // namespace sbokena::editor::tile