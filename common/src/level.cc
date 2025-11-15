#include "level.hh"

#include <variant>

#include <nlohmann/json.hpp>

#include "utils.hh"

using namespace nlohmann::json_literals;
using nlohmann::json;

using namespace sbokena::utils;

namespace sbokena::level {

// impl Deserialize for Floor
void from_json(const json &, Floor &) {
  // Floor is empty, we're done
}

// impl Serialize for Floor
void to_json(json &j, const Floor &) {
  j = "{}"_json;
}

// impl Deserialize for Button
void from_json(const json &j, Button &b) {
  j.at("pair_id").get_to(b.pair_id);
}

// impl Serialize for Button
void to_json(json &j, const Button &b) {
  j = {{"pair_id", b.pair_id}};
}

// impl Deserialize for Door
void from_json(const json &j, Door &d) {
  j.at("pair_id").get_to(d.pair_id);
  j.at("open").get_to(d.open);
}

// impl Serialize for Door
void to_json(json &j, const Door &d) {
  j = {
    {"pair_id", d.pair_id},
    {"open", d.open},
  };
}

// impl Deserialize for Portal
void from_json(const json &j, Portal &p) {
  j.at("pair_id").get_to(p.pair_id);
}

// impl Serialize for Portal
void to_json(json &j, const Portal &p) {
  j = {{"pair_id", p.pair_id}};
}

// impl Deserialize for Tile
void from_json(const json &j, Tile &t) {
  u32 type = j.at("type").get<u32>();

  constexpr usize FLOOR_TYPE = index_of<Tile, Floor>();
  constexpr usize BUTTON_TYPE = index_of<Tile, Button>();
  constexpr usize DOOR_TYPE = index_of<Tile, Door>();
  constexpr usize PORTAL_TYPE = index_of<Tile, Portal>();
  static_assert(
    index_of<Tile, Portal>() + 1 == std::variant_size_v<Tile>,
    "more variants for Tile than expected, please update this file");

  switch (type) {
  case FLOOR_TYPE:
    Floor f;
    from_json(j, f);
    t = f;
    break;
  case BUTTON_TYPE:
    Button b;
    from_json(j, b);
    t = b;
    break;
  case DOOR_TYPE:
    Door d;
    from_json(j, d);
    t = d;
    break;
  case PORTAL_TYPE:
    Portal p;
    from_json(j, p);
    t = p;
    break;
  default:
    throw std::runtime_error(std::format("unknown tile type: {}", type));
  }
}

// impl Serialize for Tile
void to_json(json &j, const Tile &t) {
  // clang-format off
  overload sers = {
    [&](const Floor &f) {
      to_json(j, f);
    },
    [&](const Button &b) {
      to_json(j, b);
    },
    [&](const Door &d) {
      to_json(j, d);
    },
    [&](const Portal &p) {
      to_json(j, p);
    },
  };
  // clang-format on

  std::visit(sers, t);
  j.push_back({"type", t.index()});
}

// impl Deserialize for Level
void from_json(const json &, Level &) {
  // TODO: level parsing
}

// impl Serialize for Level
void to_json(json &, const Level &) {
  // TODO: level writing
}

} // namespace sbokena::level
