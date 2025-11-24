#include <string_view>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "level.hh"
#include "utils.hh"

using namespace std::string_view_literals;

using namespace nlohmann::json_literals;
using nlohmann::json;

using namespace sbokena::utils;

namespace sbokena::level {

TEST(common, floor_de) {
  const auto src = "{}"sv;
  json::parse(src).get<Floor>();
}

TEST(common, floor_rt) {
  const auto f = Floor{};
  const auto fs = json(f).dump();
  json::parse(fs).get<Floor>();
  // no errors, floor is trivial
}

TEST(common, button_de) {
  const auto src = R"(
     { "door_id": 4 }
   )"sv;
  const auto b = json::parse(src).get<Button>();
  ASSERT_EQ(b.door_id, 4);
}

TEST(common, button_rt) {
  const auto b = Button{
    .door_id = 12,
  };
  const auto bs = json(b).dump();
  const auto brt = json::parse(bs).get<Button>();

  ASSERT_EQ(b.door_id, brt.door_id);
}

TEST(common, door_de) {
  const auto src = R"(
    { "door_id": 8 }
  )"sv;
  const auto d = json::parse(src).get<Door>();
  ASSERT_EQ(d.door_id, 8);
};

TEST(common, door_rt) {
  const auto d = Door{
    .door_id = 24,
  };
  const auto ds = json(d).dump();
  const auto drt = json::parse(ds).get<Door>();

  ASSERT_EQ(d.door_id, drt.door_id);
}

TEST(common, portal_de) {
  const auto src = R"(
    {
      "portal_id": 32,
      "in_dir": "Left"
    }
  )"sv;
  const auto p = json::parse(src).get<Portal>();
  ASSERT_EQ(p.portal_id, 32);
  ASSERT_EQ(p.in_dir, Direction::Left);
}

TEST(common, portal_rt) {
  const auto p = Portal{
    .portal_id = 40,
    .in_dir = Direction::Up,
  };
  const auto ps = json(p).dump();
  const auto prt = json::parse(ps).get<Portal>();

  ASSERT_EQ(p.portal_id, prt.portal_id);
  ASSERT_EQ(p.in_dir, prt.in_dir);
}

TEST(common, tile_de) {
  static_assert(index_of<Tile, Door>() == 2,
                "index of Door used in src, please update");

  const auto src = R"(
    {
      "type": 2,
      "door_id": 12
    }
  )"sv;
  const auto t = json::parse(src).get<Tile>();
  const auto tv = std::get<2>(t);

  ASSERT_EQ(tv.door_id, 12);
}

TEST(common, tile_rt) {
  const auto t = Tile{Button{
    .door_id = 7,
  }};
  const auto tv = std::get<index_of<Tile, Button>()>(t);
  const auto ts = json(t).dump();
  const auto trt = json::parse(ts).get<Tile>();
  const auto trtv = std::get<index_of<Tile, Button>()>(trt);

  ASSERT_EQ(tv.door_id, trtv.door_id);
}

} // namespace sbokena::level
