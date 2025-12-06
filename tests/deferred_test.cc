// UI tests for `Deferred`.

#include <gtest/gtest.h>

#include "types.hh"
#include "utils.hh"

using namespace sbokena::types;

namespace sbokena::utils {

TEST(common, deferred_named) {
  usize num = 0;
  ASSERT_EQ(num, 0);

  {
    Deferred _ {[&]() { ++num; }};

    num += 1;
    ASSERT_EQ(num, 1);
  }

  ASSERT_EQ(num, 2);
}

TEST(common, deferred_unnamed) {
  usize num = 0;
  ASSERT_EQ(num, 0);

  {
    Deferred {[&]() { ++num; }};

    ++num;
    ASSERT_EQ(num, 2);
  }
}

TEST(common, deferred_cancel) {
  usize num = 0;
  ASSERT_EQ(num, 0);

  {
    Deferred _ {[&]() { ++num; }};
    ASSERT_EQ(num, 0);
  }

  ASSERT_EQ(num, 1);

  {
    Deferred df {[&]() { ++num; }};
    ASSERT_EQ(num, 1);
    df.cancel();
  }

  ASSERT_EQ(num, 1);
}

} // namespace sbokena::utils
