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

} // namespace sbokena::game::state
