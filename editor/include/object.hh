// A file implementing the structure for moveable objects
// that are placed on top of files. These include the player
// and boxes, which can be pushed around by the player. To
// open a door, a box needs to be on top of its respective
// button.

#pragma once

#include "direction.hh"
#include "types.hh"

using namespace sbokena::types;
using namespace sbokena::direction;

namespace sbokena::editor::object {

// enumeration of types of object.
enum class ObjectType { Box, OneDirBox, Player };

// contains common traits of all object classes such as id
// and type.
class Object {
public:
  explicit Object(ObjectType type, u32 id) : type {type}, id {id} {}

  virtual ~Object() = default;

  ObjectType get_type() const {
    return type;
  }

  u32 get_id() const {
    return id;
  }

private:
  ObjectType type;
  u32        id;
  // TODO: an image related id.
};

// the player; starts at the tile linked to them in the
// level.
class Player : public Object {
public:
  Player(u32 id) : Object(ObjectType::Player, id) {}
};

// the box; place it on a button to open the door.
class Box : public Object {
public:
  Box(u32 id) : Object(ObjectType::Box, id) {}
};

// same as the box but can only move in one way.
class OneDirBox : public Object {
public:
  // constructor; by default the direction is up.
  OneDirBox(u32 id)
    : Object(ObjectType::OneDirBox, id),
      dir(Direction::Up) {}

  // to which only direction to move on this tile.
  Direction get_dir() const {
    return dir;
  }

  // can only move to this new direction on this tile.
  void set_dir(Direction newdir) {
    dir = newdir;
  }

  // rotate clockwise.
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

} // namespace sbokena::editor::object
