// `loader::Theme` loading and validation test.

#include <filesystem>

#include <gtest/gtest.h>
#include <raylib.h>

#include "loader.hh"
#include "utils.hh"

namespace fs = std::filesystem;

using sbokena::utils::Deferred;

namespace sbokena::loader {

// try to find and load the `dev` theme (inside `themes/dev`)
// this should work when called both from the repository root,
// as well as when called by `ctest` in `build/tests`.
TEST(common, theme_load_ok) {
  const auto         cwd {fs::current_path()};
  const Theme<Image> theme {"dev", cwd};
  // destroy it, see if unloading fails
}

} // namespace sbokena::loader
