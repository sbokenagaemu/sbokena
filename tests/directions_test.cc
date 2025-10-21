#include <gtest/gtest.h>

#include "types.hh"

namespace sbokena::types {

TEST(common, directions) {
  using Direction::Down;
  using Direction::Left;
  using Direction::Right;
  using Direction::Up;

  const Directions none;
  const Directions all = Up | Down | Left | Right;

  ASSERT_FALSE(none.contains(Direction::Up));
  ASSERT_FALSE(none.contains_all(all));
  ASSERT_FALSE(none.contains_any(all));

  // omega contains null
  ASSERT_TRUE(all.contains_all(none));
  ASSERT_TRUE(all.contains_any(none));

  const Directions left = Left;
  const Directions vert = Up | Down;
  const Directions hor = Left | Right;
  ASSERT_FALSE(none.contains_all(left));
  ASSERT_FALSE(none.contains_any(left));
  ASSERT_TRUE(all.contains_all(left));
  ASSERT_TRUE(all.contains_any(left));
  ASSERT_FALSE(vert.contains_all(left));
  ASSERT_FALSE(vert.contains_any(left));
  ASSERT_TRUE(hor.contains_all(left));
  ASSERT_TRUE(hor.contains_any(left));
  ASSERT_FALSE(left.contains_all(hor));
  ASSERT_TRUE(left.contains_any(hor));
}

} // namespace sbokena::types
