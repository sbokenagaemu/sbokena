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
    { "pair_id": 4 }
  )"sv;
  const auto b = json::parse(src).get<Button>();
  ASSERT_EQ(b.pair_id, 4);
}

TEST(common, button_rt) {
  const auto b = Button{
    .pair_id = 12,
  };
  const auto bs = json(b).dump();
  const auto brt = json::parse(bs).get<Button>();

  ASSERT_EQ(b.pair_id, brt.pair_id);
}

TEST(common, door_de) {
  const auto src = R"(
    {
      "pair_id": 8,
      "open": true
    }
  )"sv;
  const auto d = json::parse(src).get<Door>();
  ASSERT_EQ(d.pair_id, 8);
  ASSERT_TRUE(d.open);
};

TEST(common, door_rt) {
  const auto d = Door{
    .pair_id = 24,
    .open = false,
  };
  const auto ds = json(d).dump();
  const auto drt = json::parse(ds).get<Door>();

  ASSERT_EQ(d.pair_id, drt.pair_id);
  ASSERT_EQ(d.open, drt.open);
}

TEST(common, portal_de) {
  const auto src = R"(
    { "pair_id": 32 }
  )"sv;
  const auto p = json::parse(src).get<Portal>();
  ASSERT_EQ(p.pair_id, 32);
}

TEST(common, portal_rt) {
  const auto p = Portal{.pair_id = 40};
  const auto ps = json(p).dump();
  const auto prt = json::parse(ps).get<Portal>();

  ASSERT_EQ(p.pair_id, prt.pair_id);
}

TEST(common, tile_de) {
  static_assert(index_of<Tile, Door>() == 2,
                "index of Door used in src, please update");

  const auto src = R"(
    {
      "type": 2,
      "pair_id": 12,
      "open": false
    }
  )"sv;
  const auto t = json::parse(src).get<Tile>();
  const auto tv = std::get<2>(t);

  ASSERT_EQ(tv.pair_id, 12);
  ASSERT_FALSE(tv.open);
}

TEST(common, tile_rt) {
  const auto t = Tile{Button{
    .pair_id = 7,
  }};
  const auto tv = std::get<index_of<Tile, Button>()>(t);
  const auto ts = json(t).dump();
  const auto trt = json::parse(ts).get<Tile>();
  const auto trtv = std::get<index_of<Tile, Button>()>(trt);

  ASSERT_EQ(tv.pair_id, trtv.pair_id);
}

} // namespace sbokena::level
