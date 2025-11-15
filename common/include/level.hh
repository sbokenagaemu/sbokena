// level-file I/O.

#pragma once

#include <map>
#include <variant>

#include <nlohmann/json.hpp>

#include "position.hh"
#include "types.hh"

using nlohmann::json;

using namespace sbokena::types;
using sbokena::position::Position;

namespace sbokena::level {

// ===== types =====

// a regular non-wall tile.
struct Floor {};

// a button in a button-door pair.
struct Button {
  u32 pair_id;
};

// a door in a button-door pair.
struct Door {
  u32 pair_id;
  bool open;
};

// a portal in a portal-portal pair.
struct Portal {
  u32 pair_id;
};

// clang-format off

// some tile in a grid.
using Tile = std::variant<
  Floor,
  Button,
  Door,
  Portal
>;

// clang-format on

struct Level {
  std::map<Position<>, Tile> tiles;
  // FIXME: blocked on Theme type
  // std::variant<u32, Theme> theme;
};

// ===== from_json/to_json impls =====

void from_json(const json &, Floor &);
void to_json(json &, const Floor &);

void from_json(const json &, Button &);
void to_json(json &, const Button &);

void from_json(const json &, Door &);
void to_json(json &, const Door &);

void from_json(const json &, Portal &);
void to_json(json &, const Portal &);

void from_json(const json &, Tile &);
void to_json(json &, const Tile &);

void from_json(const json &, Level &);
void to_json(json &, const Level &);

}; // namespace sbokena::level
