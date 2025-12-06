// level types used in ser/de.
// see `docs/RULES.md` for the spec.

#pragma once

#include <nlohmann/json.hpp>

#include "direction.hh"
#include "position.hh"
#include "types.hh"

using nlohmann::json;

using namespace sbokena::types;
using sbokena::direction::Direction;
using sbokena::position::Position;

namespace sbokena::level {

// declares that an object has `nlohmann_json` ser/de
// functions. these must be defined in `src/level.cc`.
//
// these functions throw an exception on parsing error.
#define DECL_JSON(ty)                 \
  void from_json(const json &, ty &); \
  void to_json(json &, const ty &);

// ===== tiles =====
// NOTE: all tiles not stored in the level file are to be
// interpreted as `3.7. Walls`.

// 3.1. Floor
struct Floor {};
DECL_JSON(Floor)

// 3.2. Button
struct Button {
  u32 door_id;
};
DECL_JSON(Button)

// 3.3. Door
struct Door {
  u32 door_id;
};
DECL_JSON(Door)

// 3.4. Portal
struct Portal {
  u32 portal_id;

  // direction from which the portal may be entered.
  // `out_dir` is always the opposite of this direction.
  Direction in_dir;
};
DECL_JSON(Portal)

// 3.5. Uni-directional floor
struct DirFloor {
  // direction in which this floor may be traversed.
  Direction dir;
};
DECL_JSON(DirFloor)

// 3.6. Goal
struct Goal {};

DECL_JSON(Goal);

// 1. Tile
using Tile =
  std::variant<Floor, Button, Door, Portal, DirFloor, Goal>;
DECL_JSON(Tile)

// ===== objects =====

// 4.1. Player
struct Player {};
DECL_JSON(Player)

// 4.2. Box
struct Box {};
DECL_JSON(Box)

// 4.3. Uni-directional box
struct DirBox {
  // direction in which this box may be pushed.
  Direction dir;
};
DECL_JSON(DirBox)

// 2. Object
using Object = std::variant<Player, Box, DirBox>;
DECL_JSON(Object)

// ===== level =====

// the level's creator-selected difficulty rating.
enum struct Difficulty {
  Unknown,
  Easy,
  Medium,
  Hard,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
  Difficulty,
  {
    {Difficulty::Unknown, "Unknown"},
    {Difficulty::Easy, "Easy"},
    {Difficulty::Medium, "Medium"},
    {Difficulty::Hard, "Hard"},
  }
)

// raw level data from a level file.
//
// this type does almost no validation.
// if this is needed, see `sbokena::loader::Level`.
struct RawLevel {
  std::string name;
  std::string theme;
  Difficulty  diff;

  std::map<Position<>, Tile>   tiles;
  std::map<Position<>, Object> objects;
};
DECL_JSON(RawLevel)

#undef DECL_JSON

}; // namespace sbokena::level
