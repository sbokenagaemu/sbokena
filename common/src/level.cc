#include "level.hh"

#include <format>
#include <utility>
#include <variant>

#include <nlohmann/json.hpp>

#include "utils.hh"

using namespace nlohmann::json_literals;
using nlohmann::json;

using namespace sbokena::utils;

namespace sbokena::level {

// define from_json/to_json for a struct type.
//
// basically identical to
// `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE`, but is not generic
// over `json` and therefore works with... something,
// somehow.
//
// technically this is not a public macro, so be careful
// with updating. rewrite, if possible, but increment this
// counter below if you fail.
//
// hours_wasted_here = 5
#define JSON_IMPL_STRUCT(type, ...)                                  \
  void from_json(                                                    \
    const json &nlohmann_json_j, type &nlohmann_json_t               \
  ) {                                                                \
    NLOHMANN_JSON_EXPAND(                                            \
      NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)           \
    )                                                                \
  }                                                                  \
  void to_json(json &nlohmann_json_j, const type &nlohmann_json_t) { \
    NLOHMANN_JSON_EXPAND(                                            \
      NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)             \
    )                                                                \
  }

// define from_json/to_json for an empty struct type.
#define JSON_IMPL_EMPTY(type)             \
  void from_json(const json &, type &) {} \
  void to_json(json &j, const type &) {   \
    j = "{}"_json;                        \
  }

// deserialize a `std::variant` alternative into the
// container. this is meant to be used in a `switch` case.
#define DE_VAR(type)               \
  from_json(j, t.emplace<type>()); \
  break;

// delegated serializer of a `std::variant` alternative.
// this is meant to be used with "the overload trick".
#define SER_VAR(type) [&j](const type &v) { return to_json(j, v); }

// ===== tiles =====

JSON_IMPL_EMPTY(Floor)
JSON_IMPL_STRUCT(Button, door_id)
JSON_IMPL_STRUCT(Door, door_id)
JSON_IMPL_STRUCT(Portal, portal_id, in_dir)
JSON_IMPL_STRUCT(DirFloor, dir)
JSON_IMPL_EMPTY(Goal)

void from_json(const json &j, Tile &t) {
  constexpr usize FLOOR_TYPE    = index_of<Tile, Floor>();
  constexpr usize BUTTON_TYPE   = index_of<Tile, Button>();
  constexpr usize DOOR_TYPE     = index_of<Tile, Door>();
  constexpr usize PORTAL_TYPE   = index_of<Tile, Portal>();
  constexpr usize DIRFLOOR_TYPE = index_of<Tile, DirFloor>();
  constexpr usize GOAL_TYPE     = index_of<Tile, Goal>();
  static_assert(
    std::variant_size_v<Tile> == 6,
    "Tile gained variants, please update"
  );

  usize type = j.at("type").get<usize>();
  switch (type) {
  case FLOOR_TYPE:
    DE_VAR(Floor);
  case BUTTON_TYPE:
    DE_VAR(Button);
  case DOOR_TYPE:
    DE_VAR(Door);
  case PORTAL_TYPE:
    DE_VAR(Portal);
  case DIRFLOOR_TYPE:
    DE_VAR(DirFloor);
  case GOAL_TYPE:
    DE_VAR(Goal);
  default:
    throw std::runtime_error(
      std::format("unknown tile type: {}", type)
    );
  }
}

void to_json(json &j, const Tile &t) {
  // clang-format off
  overload ser_handlers = {
    SER_VAR(Floor),
    SER_VAR(Button),
    SER_VAR(Door),
    SER_VAR(Portal),
    SER_VAR(DirFloor),
    SER_VAR(Goal),
  };
  // clang-format on

  std::visit(ser_handlers, t);
  j.push_back({"type", t.index()});
}

// ===== objects =====

JSON_IMPL_EMPTY(Player)
JSON_IMPL_EMPTY(Box)
JSON_IMPL_STRUCT(DirBox, dir)

void from_json(const json &j, Object &t) {
  // clang-format off
  constexpr usize PLAYER_TYPE = index_of<Object, Player>();
  constexpr usize BOX_TYPE    = index_of<Object, Box>();
  constexpr usize DIRBOX_TYPE = index_of<Object, DirBox>();
  static_assert(std::variant_size_v<Object> == 3,
                "Tile gained variants, please update");
  // clang-format on

  usize type = j.at("type").get<usize>();
  switch (type) {
  case PLAYER_TYPE:
    DE_VAR(Player)
  case BOX_TYPE:
    DE_VAR(Box)
  case DIRBOX_TYPE:
    DE_VAR(DirBox)
  default:
    throw std::runtime_error(
      std::format("unknown tile type: {}", type)
    );
  }
}

void to_json(json &j, const Object &t) {
  // clang-format off
  overload ser_handlers = {
    SER_VAR(Player),
    SER_VAR(Box),
    SER_VAR(DirBox)
  };
  // clang-format on

  std::visit(ser_handlers, t);
  j.push_back({"type", t.index()});
}

// ===== level =====

void from_json(const json &j, RawLevel &l) {
  j.at("name").get_to(l.name);
  j.at("theme").get_to(l.theme);
  j.at("diff").get_to(l.diff);

  using PackedTile   = std::pair<Position<>, Tile>;
  using PackedObject = std::pair<Position<>, Object>;

  const auto tiles = j.at("tiles").get<std::vector<PackedTile>>();
  const auto objects =
    j.at("objects").get<std::vector<PackedObject>>();

  for (const auto &ptile : tiles)
    l.tiles.insert(ptile);
  for (const auto &pobject : objects)
    l.objects.insert(pobject);
}

void to_json(json &j, const RawLevel &l) {
  j = json::object({
    {"name", l.name},
    {"theme", l.theme},
    {"diff", l.diff},
    {"tiles", json::array()},
    {"objects", json::array()},
  });

  for (const auto &ptile : l.tiles)
    j.at("tiles").push_back(ptile);
  for (const auto &pobject : l.objects)
    j.at("objects").push_back(pobject);
}

#undef JSON_IMPL_STRUCT
#undef JSON_IMPL_EMPTY
#undef DE_VAR
#undef SER_VAR

} // namespace sbokena::level
