#include <gtest/gtest.h>

#include "types.hh"

namespace sbokena::types {

TEST(common, position) {
  const Position<> p1 = {56, 78};
  const Position<> p2 = {12, 34};

  const Position<> psum = {68, 112};
  const Position<> pdif = {44, 44};
  const Position<> p1tp = {78, 56};
  const Position<> p2tp = {34, 12};

  ASSERT_EQ(p1 + p2, psum);
  ASSERT_EQ(p1 - p2, pdif);
  ASSERT_EQ(p1.tposed(), p1tp);
  ASSERT_EQ(p2.tposed(), p2tp);

  Position<> p3 = {90, 12};
  Position<> padd = {100, 100};
  p3 += {10, 88};
  ASSERT_EQ(p3, padd);
}

} // namespace sbokena::types
