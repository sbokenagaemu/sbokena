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
    Deferred _{[&]() {
      num += 1;
    }};

    num += 1;
    ASSERT_EQ(num, 1);
  }

  ASSERT_EQ(num, 2);
}

TEST(common, deferred_unnamed) {
  usize num = 0;
  ASSERT_EQ(num, 0);

  {
    Deferred{[&]() {
      num += 1;
    }};

    num += 1;
    ASSERT_EQ(num, 2);
  }
}

} // namespace sbokena::utils
