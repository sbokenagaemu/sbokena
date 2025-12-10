#include "level_complete.hh"

#include <memory>
#include <string>

#include <nlohmann/json.hpp>
#include <raygui.h>
#include <raylib.h>

#include "level.hh"
#include "scene.hh"
#include "start_menu.hh"
#include "types.hh"

using namespace sbokena::types;
using sbokena::level::Difficulty;

namespace sbokena::game::scene {

LevelCompleteScene::LevelCompleteScene(
  const Level<Texture> &level, u64 moves
)
  : level {level},
    moves {moves} {}

void LevelCompleteScene::draw() const {
  ClearBackground(VIOLET);

  const usize screen_w = GetScreenWidth();
  const usize screen_h = GetScreenHeight();
  const Font  font     = GuiGetFont();

  // ===== draw title =====

  const std::string_view title           = "level completed!";
  const f32              title_font_size = 60;
  const f32              title_spacing   = 10;
  const auto [title_width, _] =
    MeasureTextEx(font, title.data(), title_font_size, title_spacing);
  const Vector2 title_pos {
    .x = screen_w / 2 - title_width / 2,
    .y = static_cast<f32>(screen_h / 5),
  };

  DrawTextEx(
    font,
    title.data(),
    title_pos,
    title_font_size,
    title_spacing,
    YELLOW
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
  const f32 info_font_size    = 24;
  const f32 info_spacing      = 5;
  const f32 info_line_spacing = 10;

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
  const f32 view_y_spacing = level_name_size.y + info_line_spacing;

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

UpdateResult LevelCompleteScene::update(Input) {
  const usize screen_w = GetScreenWidth();
  const usize screen_h = GetScreenHeight();

  // size of 1 button in view
  const Vector2 btn_size {
    .x = 250,
    .y = 100,
  };

  const Rectangle back_bounds = {
    .x      = screen_w / 2 - btn_size.x / 2,
    .y      = static_cast<f32>(screen_h) * 3 / 4,
    .width  = btn_size.x,
    .height = btn_size.y
  };

  const bool back = GuiButton(back_bounds, "back to home");

  if (back)
    return UpdateTransition {
      .next = std::unique_ptr<Scene> {new StartMenuScene},
    };

  return UpdateOk {};
}

} // namespace sbokena::game::scene
