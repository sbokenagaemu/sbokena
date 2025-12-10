#include "pause.hh"

#include <memory>
#include <string>

#include <raygui.h>
#include <raylib.h>
#include <raymath.h>

#include "gameplay.hh"
#include "level.hh"
#include "scene.hh"
#include "start_menu.hh"
#include "types.hh"

using namespace sbokena::types;
using sbokena::level::Difficulty;

namespace sbokena::game::scene {

PauseScene::PauseScene(
  const Level<Texture> &level,
  u64                   moves,
  const GameplayScene  &game_scene
)
  : level {level},
    moves {moves},
    gameplay_scene(game_scene) {}

void PauseScene::draw() const {
  ClearBackground(DARKPURPLE);

  const usize screen_w = GetScreenWidth();
  const usize screen_h = GetScreenHeight();
  const Font  font     = GuiGetFont();

  // ===== draw title =====

  const std::string_view title           = "paused";
  const f32              title_font_size = 60;
  const f32              title_spacing   = 10;
  const f32 title_width = MeasureText(title.data(), title_font_size);
  const Vector2 title_pos {
    .x = screen_w / 2 - title_width / 2,
    .y = static_cast<f32>(screen_h / 4)
  };

  DrawTextEx(
    font,
    title.data(),
    title_pos,
    title_font_size,
    title_spacing,
    WHITE
  );

  // ===== draw stats =====

  const std::string level_name =
    std::format("level: {}", level.name());
  const std::string theme =
    std::format("theme: {}", level.theme().name());
  const std::string dif        = ([&]() {
    switch (level.diff()) {
    case Difficulty::Unknown:
      return "unknown";
    case Difficulty::Easy:
      return "easy";
    case Difficulty::Medium:
      return "medium";
    case Difficulty::Hard:
      return "hard";
    }
  })();
  const std::string difficulty = std::format("difficulty: {}", dif);
  const std::string moves_count =
    std::format("moves count: {}", std::to_string(moves));
  const f32 info_font_size = 24;
  const f32 info_spacing   = 5;

  // stats text size
  const Vector2 level_name_size = MeasureTextEx(
    font, level_name.data(), info_font_size, info_spacing
  );
  const Vector2 theme_size =
    MeasureTextEx(font, theme.data(), info_font_size, info_spacing);
  const Vector2 difficulty_size = MeasureTextEx(
    font, difficulty.data(), info_font_size, info_spacing
  );
  const Vector2 moves_size = MeasureTextEx(
    font, moves_count.data(), info_font_size, info_spacing
  );

  // stats position
  const f32 view_y =
    static_cast<f32>(screen_h) / 2 - 2 * level_name_size.y;
  const f32 view_y_spacing = level_name_size.y + 10;

  const Vector2 level_name_pos {
    .x = screen_w / 2 - level_name_size.x / 2,
    .y = view_y,
  };
  const Vector2 theme_pos {
    .x = screen_w / 2 - theme_size.x / 2,
    .y = view_y + view_y_spacing,
  };
  const Vector2 difficulty_pos {
    .x = screen_w / 2 - difficulty_size.x / 2,
    .y = view_y + 2 * view_y_spacing,
  };
  const Vector2 moves_pos {
    .x = screen_w / 2 - moves_size.x / 2,
    .y = view_y + 3 * view_y_spacing,
  };

  DrawTextEx(
    font,
    level_name.data(),
    level_name_pos,
    info_font_size,
    info_spacing,
    WHITE
  );

  DrawTextEx(
    font, theme.data(), theme_pos, info_font_size, info_spacing, WHITE
  );

  DrawTextEx(
    font,
    difficulty.data(),
    difficulty_pos,
    info_font_size,
    info_spacing,
    WHITE
  );

  DrawTextEx(
    font,
    moves_count.data(),
    moves_pos,
    info_font_size,
    info_spacing,
    WHITE
  );
}

UpdateResult PauseScene::update(Input) {
  const usize screen_w = GetScreenWidth();
  const usize screen_h = GetScreenHeight();

  // size of 1 button
  const Vector2 btn_size {.x = 180, .y = 100};

  const Vector2 view_size = btn_size * Vector2 {.x = 3, .y = 1};
  const Vector2 view_pos  = {
    screen_w / 2 - view_size.x / 2, static_cast<f32>(screen_h) * 3 / 4
  };

  // ===== draw buttons =====
  const Rectangle resume_bounds = {
    .x      = view_pos.x,
    .y      = view_pos.y,
    .width  = btn_size.x,
    .height = btn_size.y
  };
  const Rectangle quit_bounds = {
    .x      = view_pos.x + 2 * btn_size.x,
    .y      = view_pos.y,
    .width  = btn_size.x,
    .height = btn_size.y
  };

  const bool resume = GuiButton(resume_bounds, "resume");
  const bool quit   = GuiButton(quit_bounds, "quit");

  // ===== handle buttons =====
  if (quit)
    return UpdateTransition {
      .next = std::unique_ptr<Scene> {new StartMenuScene},
    };

  if (resume)
    return UpdateTransition {
      .next =
        std::unique_ptr<Scene> {new GameplayScene {gameplay_scene}},
    };

  return UpdateOk {};
}

} // namespace sbokena::game::scene
