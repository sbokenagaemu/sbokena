// theme and level loader tests.

#include <filesystem>

#include <gtest/gtest.h>
#include <raylib.h>

#include "direction.hh"
#include "level.hh"
#include "loader.hh"

namespace fs = std::filesystem;

using namespace sbokena::level;
using sbokena::direction::Direction;

namespace sbokena::loader {

// ===== theme loader tests =====

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

// ===== level loader tests =====

TEST(common, level_load_full) {
  const RawLevel raw_level = {
    .name  = "test level",
    .theme = "dev",
    .diff  = level::Difficulty::Unknown,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1}, {Button {.door_id = 0}}},
        {{.x = 3, .y = 1}, {Door {.door_id = 0}}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Down,
         }}},

        {{.x = 3, .y = 2}, {Floor {}}},
        {{.x = 4, .y = 2}, {Floor {}}},
        {{.x = 5, .y = 2}, {Goal {}}},
      },
    .objects = {
      {{.x = 1, .y = 1}, {Player {}}},
      {{.x = 4, .y = 2}, {Box {}}},
    },
  };

  const Theme<Image> theme {raw_level.theme};
  const Level<Image> level {raw_level, theme};
}

TEST(common, level_load_missing_player) {
  const RawLevel raw_level = {
    .name  = "test level",
    .theme = "dev",
    .diff  = level::Difficulty::Unknown,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1}, {Button {.door_id = 0}}},
        {{.x = 3, .y = 1}, {Door {.door_id = 0}}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Down,
         }}},

        {{.x = 3, .y = 2}, {Floor {}}},
        {{.x = 4, .y = 2}, {Floor {}}},
        {{.x = 5, .y = 2}, {Goal {}}},
      },
    .objects = {
      // {{.x = 1, .y = 1}, {Player {}}},
      {{.x = 4, .y = 2}, {Box {}}},
    },
  };

  const Theme<Image> theme {raw_level.theme};

  try {
    const Level<Image> level {raw_level, theme};
  }
  // TODO: catch the actual exception type instead
  // (InvalidLevelException)
  catch (std::exception &ex) {
    return;
  }

  FAIL();
}

TEST(common, level_load_box_lt_goal) {
  const RawLevel raw_level = {
    .name  = "test level",
    .theme = "dev",
    .diff  = level::Difficulty::Unknown,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1}, {Button {.door_id = 0}}},
        {{.x = 3, .y = 1}, {Door {.door_id = 0}}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Down,
         }}},

        {{.x = 3, .y = 2}, {Floor {}}},
        {{.x = 4, .y = 2}, {Floor {}}},
        {{.x = 5, .y = 2}, {Goal {}}},
        {{.x = 6, .y = 2}, {Goal {}}},
      },
    .objects = {
      {{.x = 1, .y = 1}, {Player {}}},
      {{.x = 4, .y = 2}, {Box {}}},
    },
  };

  const Theme<Image> theme {raw_level.theme};

  try {
    const Level<Image> level {raw_level, theme};
  }
  // TODO: catch the actual exception type instead
  // (InvalidLevelException)
  catch (std::exception &ex) {
    return;
  }

  FAIL();
}

TEST(common, level_load_dangling_door_id) {
  const RawLevel raw_level = {
    .name  = "test level",
    .theme = "dev",
    .diff  = level::Difficulty::Unknown,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1},
         {Button {
           .door_id = 0, // DANGLING DOOR_ID
         }}},
        {{.x = 3, .y = 1},
         {Door {
           .door_id = 1, // DANGLING DOOR_ID
         }}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 0,
           .in_dir    = Direction::Down,
         }}},

        {{.x = 3, .y = 2}, {Floor {}}},
        {{.x = 4, .y = 2}, {Floor {}}},
        {{.x = 5, .y = 2}, {Goal {}}},
      },
    .objects = {
      {{.x = 1, .y = 1}, {Player {}}},
      {{.x = 4, .y = 2}, {Box {}}},
    },
  };

  const Theme<Image> theme {raw_level.theme};

  try {
    const Level<Image> level {raw_level, theme};
  }
  // TODO: catch the actual exception type instead
  // (InvalidLevelException)
  catch (std::exception &ex) {
    return;
  }

  FAIL();
}

TEST(common, level_load_dangling_portal_id) {
  const RawLevel raw_level = {
    .name  = "test level",
    .theme = "dev",
    .diff  = level::Difficulty::Unknown,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1}, {Button {.door_id = 0}}},
        {{.x = 3, .y = 1}, {Door {.door_id = 1}}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 0, // DANGLING PORTAL_ID
           .in_dir    = Direction::Left,
         }}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 1, // DANGLING PORTAL_ID
           .in_dir    = Direction::Down,
         }}},

        {{.x = 3, .y = 2}, {Floor {}}},
        {{.x = 4, .y = 2}, {Floor {}}},
        {{.x = 5, .y = 2}, {Goal {}}},
      },
    .objects = {
      {{.x = 1, .y = 1}, {Player {}}},
      {{.x = 4, .y = 2}, {Box {}}},
    },
  };

  const Theme<Image> theme {raw_level.theme};

  try {
    const Level<Image> level {raw_level, theme};
  }
  // TODO: catch the actual exception type instead
  // (InvalidLevelException)
  catch (std::exception &ex) {
    return;
  }

  FAIL();
}

} // namespace sbokena::loader
