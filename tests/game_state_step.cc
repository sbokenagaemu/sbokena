#include <gtest/gtest.h>

#include "direction.hh"
#include "level.hh"
#include "state.hh"

using namespace sbokena::level;
using sbokena::direction::Direction;

namespace sbokena::game::state {

TEST(game, state_walk_normal) {
  // █████
  // █ p █
  // █████
  State st = {
    .completed_goals = 0,
    .tiles =
      {{{.x = 1, .y = 1}, {Floor {}}},
       {{.x = 2, .y = 1}, {Floor {}}},
       {{.x = 3, .y = 1}, {Floor {}}}},
    .objects = {{{.x = 2, .y = 1}, {Player {}}}},
    .doors   = {},
    .portals = {},
  };

  // check construction logic
  std::get<Player>(st.objects.at({.x = 2, .y = 1}));

  // █████
  // █  p█
  // █████
  // step right, no wall
  ASSERT_EQ(st.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 3, .y = 1}));
  ASSERT_FALSE(st.objects.contains({.x = 2, .y = 1}));

  // step right, hit wall
  ASSERT_EQ(st.step(Direction::Right), StepResult::StepOnWall);
  std::get<Player>(st.objects.at({.x = 3, .y = 1}));

  // step up, hit wall
  ASSERT_EQ(st.step(Direction::Up), StepResult::StepOnWall);
  std::get<Player>(st.objects.at({.x = 3, .y = 1}));
}

TEST(game, state_walk_dirfloor) {
  // █████
  // █ p→█
  // █████
  State st = {
    .completed_goals = 0,
    .tiles =
      {{{.x = 1, .y = 1}, {Floor {}}},
       {{.x = 2, .y = 1}, {Floor {}}},
       {{.x = 3, .y = 1}, {DirFloor {.dir = Direction::Right}}}},
    .objects = {{{.x = 2, .y = 1}, {Player {}}}},
    .doors   = {},
    .portals = {},
  };

  // █████
  // █  p█
  // █████
  // step right, on DirFloor
  ASSERT_EQ(st.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 3, .y = 1}));
  ASSERT_FALSE(st.objects.contains({.x = 2, .y = 1}));

  // step down, wrong direction
  ASSERT_EQ(st.step(Direction::Down), StepResult::InvalidDirection);
  std::get<Player>(st.objects.at({.x = 3, .y = 1}));

  // step right, hit the Wall
  ASSERT_EQ(st.step(Direction::Right), StepResult::StepOnWall);
  std::get<Player>(st.objects.at({.x = 3, .y = 1}));
}

TEST(game, state_push_normal) {
  // █████
  // █p☐ █
  // █████
  State st = {
    .completed_goals = 0,
    .tiles =
      {{{.x = 1, .y = 1}, {Floor {}}},
       {{.x = 2, .y = 1}, {Floor {}}},
       {{.x = 3, .y = 1}, {Floor {}}}},
    .objects =
      {{{.x = 1, .y = 1}, {Player {}}}, {{.x = 2, .y = 1}, {Box {}}}},
    .doors   = {},
    .portals = {},
  };

  // █████
  // █ p☐█
  // █████
  // step right, push box normally
  ASSERT_EQ(st.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 2, .y = 1}));
  std::get<Box>(st.objects.at({.x = 3, .y = 1}));

  // step right, push box on wall
  ASSERT_EQ(st.step(Direction::Right), StepResult::StepOnWall);
  std::get<Player>(st.objects.at({.x = 2, .y = 1}));
  std::get<Box>(st.objects.at({.x = 3, .y = 1}));

  // █████
  // █p ☐█
  // █████
  // step left, detach from box
  ASSERT_EQ(st.step(Direction::Left), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 1, .y = 1}));
  std::get<Box>(st.objects.at({.x = 3, .y = 1}));
}

TEST(game, state_push_dirbox) {
  // █████
  // █p͔͔͕͕˃ █
  // █████
  State st = {
    .completed_goals = 0,
    .tiles =
      {{{.x = 1, .y = 1}, {Floor {}}},
       {{.x = 2, .y = 1}, {Floor {}}},
       {{.x = 3, .y = 1}, {Floor {}}}},
    .objects =
      {{{.x = 1, .y = 1}, {Player {}}},
       {{.x = 2, .y = 1}, {DirBox {.dir = Direction::Right}}}},
    .doors   = {},
    .portals = {},
  };

  // step right, push box right direction
  ASSERT_EQ(st.step(Direction::Right), StepResult::Ok);
  ASSERT_EQ(st.objects.size(), 2);
  std::get<Player>(st.objects.at({.x = 2, .y = 1}));
  std::get<DirBox>(st.objects.at({.x = 3, .y = 1}));
  ASSERT_FALSE(st.objects.contains({.x = 1, .y = 1}));

  // step right, push box on wall
  ASSERT_EQ(st.step(Direction::Right), StepResult::StepOnWall);
  std::get<Player>(st.objects.at({.x = 2, .y = 1}));
  std::get<DirBox>(st.objects.at({.x = 3, .y = 1}));

  // step left, detach from box
  ASSERT_EQ(st.step(Direction::Left), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 1, .y = 1}));
  std::get<DirBox>(st.objects.at({.x = 3, .y = 1}));

  // █████
  // █p͔͔͕͕˄ █
  // █████
  State st_op = {
    .completed_goals = 0,
    .tiles =
      {{{.x = 1, .y = 1}, {Floor {}}},
       {{.x = 2, .y = 1}, {Floor {}}},
       {{.x = 3, .y = 1}, {Floor {}}}},
    .objects =
      {{{.x = 1, .y = 1}, {Player {}}},
       {{.x = 2, .y = 1}, {DirBox {.dir = Direction::Up}}}},
    .doors   = {},
    .portals = {},
  };

  // step right, push box wrong direction
  ASSERT_EQ(
    st_op.step(Direction::Right), StepResult::InvalidDirection
  );
  std::get<Player>(st_op.objects.at({.x = 1, .y = 1}));
  std::get<DirBox>(st_op.objects.at({.x = 2, .y = 1}));
}

TEST(game, state_door_button) {
  // █████
  // █p͔͔͕͕bd█
  // █████
  State st = {
    .completed_goals = 0,
    .tiles =
      {{{.x = 1, .y = 1}, {Floor {}}},
       {{.x = 2, .y = 1}, {Button {.door_id = 1}}},
       {{.x = 3, .y = 1}, {Door {.door_id = 1}}}},
    .objects =
      {
        {{.x = 1, .y = 1}, {Player {}}},
      },
    .doors   = {{1, {{3, 1}, {{2, 1}}}}},
    .portals = {},
  };

  // █████
  // █ pd█
  // █████
  // step right, on button
  ASSERT_EQ(st.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 2, .y = 1}));
  ASSERT_FALSE(st.objects.contains({.x = 1, .y = 1}));

  // █████
  // █ bp█
  // █████
  // step right, through door which is currently opened
  ASSERT_EQ(st.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 3, .y = 1}));
  ASSERT_FALSE(st.objects.contains({.x = 2, .y = 1}));

  // █████
  // █ pd█
  // █████
  // step left, exit door
  ASSERT_EQ(st.step(Direction::Left), StepResult::Ok);
  std::get<Player>(st.objects.at({.x = 2, .y = 1}));
  ASSERT_FALSE(st.objects.contains({.x = 3, .y = 1}));

  // █████
  // █p͔͔͕͕db█
  // █████
  State st1 = {
    .completed_goals = 0,
    .tiles =
      {{{.x = 1, .y = 1}, {Floor {}}},
       {{.x = 2, .y = 1}, {Door {.door_id = 1}}},
       {{.x = 3, .y = 1}, {Button {.door_id = 1}}}},
    .objects =
      {
        {{.x = 1, .y = 1}, {Player {}}},
      },
    .doors   = {{1, {{2, 1}, {{3, 1}}}}},
    .portals = {},
  };
  // step right, through door which is currently closed
  ASSERT_EQ(st1.step(Direction::Right), StepResult::SlamOnDoor);
  std::get<Player>(st1.objects.at({.x = 1, .y = 1}));
}

TEST(game, state_door_button_box) {
  // ███████
  // █p☐bd █
  // ███████
  State st1 = {
    .completed_goals = 0,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1}, {Floor {}}},
        {{.x = 3, .y = 1}, {Button {.door_id = 1}}},
        {{.x = 4, .y = 1}, {Door {.door_id = 1}}},
        {{.x = 5, .y = 1}, {Floor {}}},

      },
    .objects =
      {
        {{.x = 1, .y = 1}, {Player {}}},
        {{.x = 2, .y = 1}, {Box {}}},
      },
    .doors   = {{1, {{4, 1}, {{3, 1}}}}},
    .portals = {},
  };

  // ███████
  // █ p☐d █
  // ███████
  // push box on button
  ASSERT_EQ(st1.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st1.objects.at({.x = 2, .y = 1}));
  std::get<Box>(st1.objects.at({.x = 3, .y = 1}));

  // ███████
  // █  p☐ █
  // ███████
  // push box through door
  ASSERT_EQ(st1.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st1.objects.at({.x = 3, .y = 1}));
  std::get<Box>(st1.objects.at({.x = 4, .y = 1}));

  // ███████
  // █  bp☐█
  // ███████
  // push box of door, player under door
  ASSERT_EQ(st1.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st1.objects.at({.x = 4, .y = 1}));
  std::get<Box>(st1.objects.at({.x = 5, .y = 1}));
}

TEST(game, state_walk_portal) {
  // ███████
  // █p┤ ├ █
  // ███████
  State st1 = {
    .completed_goals = 0,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Right,
         }}},
        {{.x = 3, .y = 1}, {Floor {}}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 5, .y = 1}, {Floor {}}},

      },
    .objects =
      {
        {{.x = 1, .y = 1}, {Player {}}},
      },
    .doors   = {},
    .portals = {{1, {{.x = 2, .y = 1}, {.x = 4, .y = 1}}}},
  };

  // ███████
  // █ ┤ ├p█
  // ███████
  // traverse through portal
  ASSERT_EQ(st1.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st1.objects.at({.x = 5, .y = 1}));
  ASSERT_FALSE(st1.objects.contains({.x = 1, .y = 1}));

  // ███████
  // █ ┤p├ █
  // ███████
  State st2 = {
    .completed_goals = 0,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Right,
         }}},
        {{.x = 3, .y = 1}, {Floor {}}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 5, .y = 1}, {Floor {}}},

      },
    .objects =
      {
        {{.x = 3, .y = 1}, {Player {}}},
      },
    .doors   = {},
    .portals = {{1, {{.x = 2, .y = 1}, {.x = 4, .y = 1}}}},
  };

  // traverse through portal in wrong direction
  ASSERT_EQ(st2.step(Direction::Right), StepResult::InvalidDirection);
  std::get<Player>(st2.objects.at({.x = 3, .y = 1}));
  ASSERT_EQ(st2.step(Direction::Left), StepResult::InvalidDirection);
  std::get<Player>(st2.objects.at({.x = 3, .y = 1}));

  // ████████
  // █p┤├┤├ █
  // ████████
  State st3 = {
    .completed_goals = 0,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Right,
         }}},
        {{.x = 3, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 2,
           .in_dir    = Direction::Right,
         }}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 2,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 6, .y = 1}, {Floor {}}},
      },
    .objects =
      {
        {{.x = 1, .y = 1}, {Player {}}},
      },
    .doors   = {},
    .portals = {
      {1, {{.x = 2, .y = 1}, {.x = 3, .y = 1}}},
      {2, {{.x = 4, .y = 1}, {.x = 5, .y = 1}}}
    },
  };

  // ████████
  // █ ┤├┤├p█
  // ████████
  // traverse 2 portals
  ASSERT_EQ(st3.step(Direction::Right), StepResult::Ok);
  std::get<Player>(st3.objects.at({.x = 6, .y = 1}));
  ASSERT_FALSE(st3.objects.contains({.x = 1, .y = 1}));

  // ████████
  // █p┤├┬├ █
  // ████████
  State st4 = {
    .completed_goals = 0,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Right,
         }}},
        {{.x = 3, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 2,
           .in_dir    = Direction::Down,
         }}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 2,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 6, .y = 1}, {Floor {}}},
      },
    .objects =
      {
        {{.x = 1, .y = 1}, {Player {}}},
      },
    .doors   = {},
    .portals = {
      {1, {{.x = 2, .y = 1}, {.x = 3, .y = 1}}},
      {2, {{.x = 4, .y = 1}, {.x = 5, .y = 1}}}
    },
  };

  // traverse a portal into another portal in wrong direction
  ASSERT_EQ(st4.step(Direction::Right), StepResult::InvalidDirection);
  std::get<Player>(st4.objects.at({.x = 1, .y = 1}));

  // ███████
  // █ ├p┤ █
  // ███████
  State st5 = {
    .completed_goals = 0,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 3, .y = 1}, {Floor {}}},
        {{.x = 4, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Right,
         }}},
        {{.x = 5, .y = 1}, {Floor {}}},

      },
    .objects =
      {
        {{.x = 3, .y = 1}, {Player {}}},
      },
    .doors   = {},
    .portals = {{1, {{.x = 2, .y = 1}, {.x = 4, .y = 1}}}},
  };

  // traverse through portal in loop, encounter self
  ASSERT_EQ(st5.step(Direction::Right), StepResult::PushYourself);
  std::get<Player>(st5.objects.at({.x = 3, .y = 1}));
  ASSERT_EQ(st5.step(Direction::Left), StepResult::PushYourself);
  std::get<Player>(st5.objects.at({.x = 3, .y = 1}));
}

TEST(game, state_push_portal) {
  // █████████
  // █p☐┤ ├  █
  // █████████
  State st1 = {
    .completed_goals = 0,
    .tiles =
      {
        {{.x = 1, .y = 1}, {Floor {}}},
        {{.x = 2, .y = 1}, {Floor {}}},
        {{.x = 3, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Right,
         }}},
        {{.x = 4, .y = 1}, {Floor {}}},
        {{.x = 5, .y = 1},
         {Portal {
           .portal_id = 1,
           .in_dir    = Direction::Left,
         }}},
        {{.x = 6, .y = 1}, {Floor {}}},
        {{.x = 7, .y = 1}, {Floor {}}},

      },
    .objects =
      {
        {{.x = 1, .y = 1}, {Player {}}},
        {{.x = 2, .y = 1}, {Box {}}},

      },
    .doors   = {},
    .portals = {{1, {{.x = 3, .y = 1}, {.x = 5, .y = 1}}}},
  };

  ASSERT_EQ(st1.step(Direction::Right), StepResult::Ok);
  ASSERT_FALSE(st1.objects.contains({.x = 1, .y = 1}));
  std::get<Player>(st1.objects.at({.x = 2, .y = 1}));
  std::get<Box>(st1.objects.at({.x = 5, .y = 1}));
}

// TODO: test multiple buttons and one door

} // namespace sbokena::game::state
