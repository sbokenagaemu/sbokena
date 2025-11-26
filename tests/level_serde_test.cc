#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "direction.hh"
#include "level.hh"
#include "utils.hh"

#include <string_view>

using namespace std::string_view_literals;

using namespace nlohmann::json_literals;
using nlohmann::json;

using namespace sbokena::utils;
using sbokena::direction::Direction;

namespace sbokena::level {

TEST(common, empty_ser) {
  const json j = Floor {};
  ASSERT_EQ(j.dump(), "{}"sv);
}

TEST(common, empty_de) {
  const json j = json::parse("{}"sv);
  j.get<Floor>();
}

TEST(common, struct_ser) {
  const json j = Portal {
    .portal_id = 12,
    .in_dir    = Direction::Down,
  };
  ASSERT_EQ(j.dump(), R"({"in_dir":"Down","portal_id":12})"sv);
}

TEST(common, struct_de) {
  const json   j = json::parse(R"(
    {
      "portal_id": 3,
      "in_dir": "Left"
    }
  )"sv);
  const Portal p = j.get<Portal>();
  ASSERT_EQ(p.portal_id, 3);
  ASSERT_EQ(p.in_dir, Direction::Left);
}

TEST(common, tile_ser) {
  const json j = Tile {Portal {
    .portal_id = 4,
    .in_dir    = Direction::Up,
  }};
  static_assert(
    index_of<Tile, Portal>() == 3, "index of Portal in Tile changed"
  );
  ASSERT_EQ(j.dump(), R"({"in_dir":"Up","portal_id":4,"type":3})"sv);
}

TEST(common, tile_de) {
  const json   j = json::parse(R"(
    {
      "type": 3,
      "portal_id": 2,
      "in_dir": "Right"
    }
  )");
  const Tile   t = j.get<Tile>();
  const Portal p = std::get<Portal>(t);
  ASSERT_EQ(p.portal_id, 2);
  ASSERT_EQ(p.in_dir, Direction::Right);
}

TEST(common, level_ser) {
  const json j = Level {
    .name  = "test level",
    .theme = "dev",
    .diff  = Difficulty::Unknown,
    .tiles =
      {
        {{.x = 0, .y = 0}, {Floor {}}},
        {{.x = 1, .y = 0}, {Goal {}}},
        {{.x = 0, .y = 1}, {Button {.door_id = 0}}},
        {{.x = 1, .y = 1}, {Door {.door_id = 0}}},
      },
    .objects = {
      {{.x = 0, .y = 0}, {Player {}}},
      {{.x = 0, .y = 1}, {Box {}}},
    },
  };

  ASSERT_EQ(
    j.dump(),
    // clang-format off
    "{"
      R"("diff":"Unknown",)"
      R"("name":"test level",)"
      R"("objects":[)"
        R"([{"x":0,"y":0},)"
        R"({"type":0}],)"
        R"([{"x":0,"y":1},)"
        R"({"type":1}])"
      R"(],)"
      R"("theme":"dev",)"
      R"("tiles":[)"
        R"([{"x":0,"y":0},)"
        R"({"type":0}],)"
        R"([{"x":0,"y":1},)"
        R"({"door_id":0,"type":1}],)"
        R"([{"x":1,"y":0},)"
        R"({"type":5}],)"
        R"([{"x":1,"y":1},)"
        R"({"door_id":0,"type":2}])"
      "]"
    "}"sv
    // clang-format on
  );
}

TEST(common, level_de) {
  static_assert(
    index_of<Tile, Portal>() == 3, "index of Portal in Tile changed"
  );
  static_assert(
    index_of<Object, Player>() == 0,
    "index of Player in Object changed"
  );
  static_assert(
    index_of<Object, DirBox>() == 2,
    "index of DirBox in Object changed"
  );

  const json  j = json::parse(R"(
    {
      "name": "test level 2",
      "theme": "jungle",
      "diff": "Medium",
      "tiles": [
        [{"x": 0, "y": 0}, {"type": 3, "portal_id": 4, "in_dir": "Down"}],
        [{"x": 0, "y": 1}, {"type": 3, "portal_id": 4, "in_dir": "Up"}]
      ],
      "objects": [
        [{"x": 1, "y": 0}, {"type": 0}],
        [{"x": 1, "y": 1}, {"type": 2, "dir": "Left"}]
      ]
    }
  )"sv);
  const Level l = j.get<Level>();

  ASSERT_EQ(l.name, "test level 2"sv);
  ASSERT_EQ(l.theme, "jungle"sv);
  ASSERT_EQ(l.diff, Difficulty::Medium);

  const Tile t00 = l.tiles.at({.x = 0, .y = 0});
  const Tile t01 = l.tiles.at({.x = 0, .y = 1});
  ASSERT_EQ(std::get<Portal>(t00).portal_id, 4);
  ASSERT_EQ(std::get<Portal>(t00).in_dir, Direction::Down);
  ASSERT_EQ(std::get<Portal>(t01).portal_id, 4);
  ASSERT_EQ(std::get<Portal>(t01).in_dir, Direction::Up);

  const Object o10 = l.objects.at({.x = 1, .y = 0});
  const Object o11 = l.objects.at({.x = 1, .y = 1});
  std::get<Player>(o10);
  ASSERT_EQ(std::get<DirBox>(o11).dir, Direction::Left);
}

} // namespace sbokena::level
