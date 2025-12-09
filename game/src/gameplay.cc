#include "gameplay.hh"

#include <algorithm>
#include <format>
#include <string_view>

#include <raygui.h>
#include <raylib.h>
#include <raymath.h>

#include "loader.hh"
#include "scene.hh"
#include "state.hh"

using namespace std::string_view_literals;

using sbokena::loader::Level;

namespace sbokena::game::scene {

GameplayScene::GameplayScene(const Level<Texture> &level)
  : state {level},
    theme {level.theme()} {}

void GameplayScene::draw() const {
  // TODO: show level background art?
  ClearBackground(DARKGREEN);

  draw_viewport();
  draw_hud();
}

UpdateResult GameplayScene::update(Input in) {
  switch (in) {
  case Input::INPUT_UP:
    state.step(Direction::Up);
    ++moves;
    break;
  case Input::INPUT_DOWN:
    state.step(Direction::Down);
    ++moves;
    break;
  case Input::INPUT_LEFT:
    state.step(Direction::Left);
    ++moves;
    break;
  case Input::INPUT_RIGHT:
    state.step(Direction::Right);
    ++moves;
    break;

  case Input::INPUT_MENU:
  case Input::INPUT_BACK:
    // TODO: transition to pause menu
    break;

  // these don't do anything in gameplay
  case Input::INPUT_NONE:
  case Input::INPUT_ENTER:
    break;
  }

  return UpdateOk {};
}

void GameplayScene::draw_viewport() const {}

void GameplayScene::draw_hud() const {
  const Font font = GuiGetFont();

  const Vector2 ctr_view_pos {
    .x = 40,
    .y = 40,
  };
  const f32   ctr_font_size    = 24;
  const f32   ctr_spacing      = 6;
  const f32   ctr_line_spacing = 5;
  const f32   ctr_view_spacing = 15;
  const Color ctr_color        = WHITE;

  // ===== draw move counter =====

  const std::string_view move_ctr = "moves"sv;
  const std::string      move_cnt = std::format("{}", moves);

  const Vector2 move_ctr_size =
    MeasureTextEx(font, move_ctr.data(), ctr_font_size, ctr_spacing);
  const Vector2 move_cnt_size =
    MeasureTextEx(font, move_cnt.data(), ctr_font_size, ctr_spacing);
  const Vector2 move_ctr_view_size {
    .x = std::max(move_ctr_size.x, move_cnt_size.x),
    .y = std::max(move_ctr_size.y, move_cnt_size.y),
  };

  const Vector2 move_ctr_pos = ctr_view_pos;
  const Vector2 move_cnt_pos {
    .x = move_ctr_pos.x,
    .y = move_ctr_pos.y + move_ctr_size.y + ctr_line_spacing,
  };

  DrawTextEx(
    font,
    move_ctr.data(),
    move_ctr_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );
  DrawTextEx(
    font,
    move_cnt.data(),
    move_cnt_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );

  // ===== draw goal counter =====

  const std::string_view goal_ctr = "goals"sv;
  const std::string      goal_cnt = std::format("{}", 0);

  const Vector2 goal_ctr_size =
    MeasureTextEx(font, goal_ctr.data(), ctr_font_size, ctr_spacing);

  const Vector2 goal_ctr_pos {
    .x = ctr_view_pos.x + move_ctr_view_size.x + ctr_view_spacing,
    .y = ctr_view_pos.y,
  };
  const Vector2 goal_cnt_pos {
    .x = goal_ctr_pos.x,
    .y = goal_ctr_pos.y + goal_ctr_size.y + ctr_line_spacing,
  };

  DrawTextEx(
    font,
    goal_ctr.data(),
    goal_ctr_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );
  DrawTextEx(
    font,
    goal_cnt.data(),
    goal_cnt_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );
}

}; // namespace sbokena::game::scene
