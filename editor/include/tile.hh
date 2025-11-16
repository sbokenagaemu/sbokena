// A class implementing the tiles in the grid inside the editor. The tile
// contains its absolute/relative position within the grid (still contested),
// the list of objects above it (for example, the character or the box above the
// floor), and its sprite image.

#pragma once

#include "direction.hh"
#include "position.hh"
#include "types.hh"

using namespace sbokena::types;
using namespace sbokena::position;
using namespace sbokena::direction;

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

// An abstract class contaning common fields and functions that all tiles share.
class Tile {
public:
  // construct a new tile, contains no object by default
  explicit Tile(TileType type, u32 id, Position<> pos)
    : type(type), id(id), pos(pos), obj_id(0x00) {}

  // default destructor
  virtual ~Tile() = default;

  // returns tile type
  TileType get_type() const {
    return type;
  }

  // tile's id
  u32 get_id() const {
    return id;
  }

  // tile's pos
  Position<> get_pos() const {
    return pos;
  }

  // if there is an object on the tile
  virtual bool contains_obj() const {
    return (obj_id != 0x00);
  }

  // tile's object id
  virtual u32 get_obj_id() const {
    return obj_id;
  }

  // object goes on tile
  virtual void set_obj_id(u32 &new_id) {
    obj_id = new_id;
  }

  // object goes off tile
  virtual void remove_obj_id() {
    obj_id = 0x00;
  }

private:
  TileType type;
  u32 id;
  Position<> pos;
  u32 obj_id;
};

// Foward declarations.
class Portal;
class Door;
class Button;

// The default type for a tile. Does not and cannot contain any object.
class Wall : public Tile {
public:
  // construct a new wall
  explicit Wall(u32 id, Position<> pos) : Tile(TileType::Wall, id, pos) {}

  // does not contain object
  bool contains_obj() const override {
    return false;
  }

  // no object, no id
  u32 get_obj_id() const override {
    return 0x00;
  }

  // object cannot be on wall
  void set_obj_id(u32 &) override {}

  // does not have object to remove to begin with
  void remove_obj_id() override {}
};

// The basic, most common type of non-wall tile. Can contain an object.
class Floor : public Tile {
public:
  // construct a new floor
  Floor(u32 id, Position<> pos) : Tile(TileType::Floor, id, pos) {}
};

// Similar to floor but only lets object move through one specified way.
class OneDir : public Tile {
public:
  // construct a new one direction floor
  // by default the direction is up
  OneDir(u32 id, Position<> pos)
    : Tile(TileType::OneDir, id, pos), dir(Direction::Up) {}

  // to which only direction to move on this tile
  Direction get_dir() const {
    return dir;
  }

  // can only move to this new direction on this tile
  void set_dir(Direction newdir) {
    dir = newdir;
  }

  // rotate clockwise
  void rotate() {
    switch (dir) {
    case Direction::Up:
      dir = Direction::Right;
      break;
    case Direction::Right:
      dir = Direction::Down;
      break;
    case Direction::Down:
      dir = Direction::Left;
      break;
    case Direction::Left:
      dir = Direction::Up;
      break;
    }
  }

private:
  Direction dir;
};

// If the player arrives into this tile, the level is completed.
class Goal : public Tile {
public:
  Goal(u32 id, Position<> pos) : Tile(TileType::Goal, id, pos) {}
};

// Must be linked to another portal to be valid.
// Can teleport objects to the linked portal.
class Portal : public Tile {
public:
  // construct a new portal, by default isn't linked to another portal
  Portal(u32 id, Position<> pos)
    : Tile(TileType::Portal, id, pos), linked_id(0x00) {}

  // the linked portal
  u32 get_linked() const {
    return linked_id;
  }

  // link a portal
  void set_linked(u32 &new_id) {
    linked_id = new_id;
  }

  // unlink a portal
  void remove_linked() {
    linked_id = 0x00;
  }

  // valid when linked to another portal
  bool is_valid() const {
    return (linked_id != 0x00);
  }

private:
  u32 linked_id;
};

// Must be linked to another button to be valid.
// Opens when a the linked button is pressed.
class Door : public Tile {
public:
  // construct a new door, by default isn't linked to another button
  Door(u32 id, Position<> pos)
    : Tile(TileType::Door, id, pos), button_id(0x00), opened(false) {}

  // the linked button
  u32 get_linked() const {
    return button_id;
  }

  // link a button
  void set_linked(u32 &new_id) {
    button_id = new_id;
  }

  // unlink a button
  void remove_linked() {
    button_id = 0x00;
  }

  // valid when linked to another button
  bool is_valid() const {
    return (button_id != 0x00);
  }

  // open the door
  void open() {
    opened = true;
  }

  // close the door
  void close() {
    opened = false;
  }

  // is the door opened or closed
  bool is_opened() const {
    return opened;
  }

private:
  u32 button_id;
  bool opened;
};

// Must be linked to another door to be valid.
// When contains an object (pressed), opens the linked door.
class Button : public Tile {
public:
  // construct a new button, by default isn't linked to another door
  Button(u32 id, Position<> pos)
    : Tile(TileType::Button, id, pos), door_id(0x00) {}

  // the linked door
  u32 get_linked() const {
    return door_id;
  }

  // link a door
  void set_linked(u32 &new_id) {
    door_id = new_id;
  }

  // unlink a door
  void remove_linked() {
    door_id = 0x00;
  }

  // valid when linked to another door
  bool is_valid() const {
    return (door_id != 0x00);
  }

private:
  u32 door_id;
};
