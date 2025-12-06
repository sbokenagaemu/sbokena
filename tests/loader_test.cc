// theme and level loader tests.

#include <filesystem>

#include <gtest/gtest.h>
#include <raylib.h>

#include "loader.hh"

namespace fs = std::filesystem;

namespace sbokena::loader {

// try to find and load the `dev` theme (inside `themes/dev`)
// this should work when called both from the repository root,
// as well as when called by `ctest` in `build/tests`.
TEST(common, theme_load_dev) {
  const fs::path     cwd {fs::current_path()};
  const Theme<Image> theme {"dev", cwd};
  const Theme<Image> copy {theme};
  const Theme<Image> move {std::move(theme)};
  const Image &_ = *theme.sprites()[Theme<Image>::__SPRITES - 1];
  // destroy it, see if unloading fails
}

} // namespace sbokena::loader
