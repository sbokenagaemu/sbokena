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

TEST(common, empty_ser) {
  const json j = Floor{};
  ASSERT_EQ(j.dump(), "{}");
}

TEST(common, empty_de) {
  const json j = json::parse("{}"sv);
  j.get<Floor>();
}

TEST(common, struct_ser) {
  const json j = Portal{
    .portal_id = 12,
    .in_dir = Direction::Down,
  };
  ASSERT_EQ(j.dump(), R"({"in_dir":"Down","portal_id":12})"sv);
}

TEST(common, struct_de) {
  const json j = json::parse(R"(
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
  const json j = Tile{Portal{
    .portal_id = 4,
    .in_dir = Direction::Up,
  }};
  static_assert(index_of<Tile, Portal>() == 3,
                "index of Portal in Tile changed");
  ASSERT_EQ(j.dump(), R"({"in_dir":"Up","portal_id":4,"type":3})"sv);
}

TEST(common, tile_de) {
  const json j = json::parse(R"(
    {
      "type": 3,
      "portal_id": 2,
      "in_dir": "Right"
    }
  )");
  const Tile t = j.get<Tile>();
  const Portal p = std::get<Portal>(t);
  ASSERT_EQ(p.portal_id, 2);
  ASSERT_EQ(p.in_dir, Direction::Right);
}

} // namespace sbokena::level
