#include "start_menu.hh"

#include <algorithm>
#include <exception>
#include <fstream>
#include <print>
#include <sstream>

#include <nlohmann/json.hpp>
#include <raygui.h>
#include <raylib.h>
#include <raymath.h>

#include "level.hh"
#include "loader.hh"
#include "scene.hh"
#include "types.hh"
#include "utils.hh"

using namespace std::string_view_literals;

using nlohmann::json;

using namespace sbokena::level;
using namespace sbokena::types;
using namespace sbokena::utils;
using namespace sbokena::loader;

namespace sbokena::game::scene {

void StartMenuScene::draw() const {
  ClearBackground(VIOLET);

  const usize screen_w = GetScreenWidth();
  const usize screen_h = GetScreenHeight();
  const Font  font     = GuiGetFont();

  // ===== draw title =====

  const std::string_view title           = "sbokena"sv;
  const f32              title_font_size = 60;
  const f32              title_spacing   = 10;
  Vector2                title_size =
    MeasureTextEx(font, title.data(), title_font_size, title_spacing);
  Vector2 title_pos {
    screen_w / 2 - title_size.x / 2,
    static_cast<float>(screen_h / 5),
  };

  DrawTextEx(
    font,
    title.data(),
    title_pos,
    title_font_size,
    title_spacing,
    WHITE
  );
}

UpdateResult StartMenuScene::update(Input) {
  const usize screen_w = GetScreenWidth();
  const usize screen_h = GetScreenHeight();

  // size of 1 button in view
  const Vector2 view_btn_size {
    .x = std::min(300.0f, static_cast<f32>(screen_w) / 6),
    .y = std::min(160.0f, static_cast<f32>(screen_h) / 8),
  };

  const Vector2 view_center {
    .x = static_cast<float>(screen_w / 2),
    .y = static_cast<float>(screen_h * 2 / 3),
  };
  const Vector2 view_size = view_btn_size * Vector2 {.x = 1, .y = 3};
  const Vector2 view_pos  = view_center - view_size / 2;

  // ===== draw buttons =====

  const Rectangle play_bounds = {
    .x      = view_pos.x,
    .y      = view_pos.y,
    .width  = view_btn_size.x * 0.9f,
    .height = view_btn_size.y * 0.9f,
  };
  const Rectangle load_bounds = {
    .x      = view_pos.x,
    .y      = view_pos.y + view_btn_size.y,
    .width  = view_btn_size.x * 0.9f,
    .height = view_btn_size.y * 0.9f,
  };
  const Rectangle quit_bounds = {
    .x      = view_pos.x,
    .y      = view_pos.y + 2 * view_btn_size.y,
    .width  = view_btn_size.x * 0.9f,
    .height = view_btn_size.y * 0.9f,
  };

  const bool play = GuiButton(play_bounds, "play");
  const bool load = GuiButton(load_bounds, "load");
  const bool quit = GuiButton(quit_bounds, "quit");

  // ===== handle buttons =====

  if (play)
    // TODO: transition to level select
    return UpdateOk {};

  if (load) {
    const auto _path = open_file_dialog();
    if (!_path)
      return UpdateOk {};
    const auto path = _path.value();

    try {
      std::ifstream     file {path};
      std::stringstream buf;
      buf << file.rdbuf();

      const RawLevel     raw_level = json::parse(buf.str());
      const Theme<Image> theme {raw_level.name, path.parent_path()};
      const Level<Image> level {raw_level, theme};
    } catch (std::exception &) {
      // TODO: show error messages to the user
      // (can probably be console logs, tbh)
      return UpdateOk {};
    }

    // TODO: transition to gameplay
    std::println("loaded level {}", path.c_str());
    return UpdateOk {};
  }

  if (quit)
    return UpdateClose {};

  return UpdateOk {};
}

} // namespace sbokena::game::scene
