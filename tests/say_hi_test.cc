#include <gtest/gtest.h>

#include "common.hh"

TEST(common, say_hi) {
  say_hi();
  EXPECT_EQ(1 == (3 - 2), true);
}
