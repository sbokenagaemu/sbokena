#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "position.hh"
#endif

#include <string>

#include <raygui.h>
#include <raylib.h>

#include "types.hh"
#include "utils.hh"

using namespace sbokena::types;
using namespace sbokena::position;
using namespace sbokena::utils;

// Minimum window size
constexpr usize INIT_WIDTH = 800;
constexpr usize INIT_HEIGHT = 600;
constexpr std::string INIT_TITLE = "sbokena";

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(INIT_WIDTH, INIT_HEIGHT, INIT_TITLE.c_str());
  SetWindowMinSize(INIT_WIDTH, INIT_HEIGHT);
  Deferred _{CloseWindow};

  while (!WindowShouldClose()) {
    BeginDrawing();
    Deferred _{EndDrawing};

    const usize window_width = GetScreenWidth();
    const usize window_height = GetScreenHeight();

    // Size and position of title
    Vector2 title_size = MeasureTextEx(GuiGetFont(), "Sbokena", 60, 10);
    Vector2 title_pos = {window_width / 2 - title_size.x / 2,
                         static_cast<float>(window_height / 4)};

    // Size and position of the view containing buttons
    Vector2 view_size = {180, 300};
    Vector2 view_pos = {window_width / 2 - view_size.x / 2,
                        window_height / 2 - view_size.y / 2 + 100};
    // Size of buttons
    Vector2 btn_size = {180, 90};

    Rectangle exit_btn = {static_cast<float>(window_width) - 20, 0, 20, 20};
    Rectangle start_btn = {view_pos.x, view_pos.y, btn_size.x, btn_size.y};
    Rectangle custom_btn = {view_pos.x, view_pos.y + btn_size.y + 10,
                            btn_size.x, btn_size.y};
    Rectangle setting_btn = {view_pos.x, view_pos.y + 2 * btn_size.y + 20,
                             btn_size.x, btn_size.y};

    ClearBackground(VIOLET);

    // Title
    DrawTextEx(GuiGetFont(), "Sbokena", title_pos, 60, 10, RED);

    // Start, Custom and Setting button
    GuiButton(start_btn, "Start game");
    GuiButton(custom_btn, "Custom");
    GuiButton(setting_btn, "Setting");

    // Exit button
    bool state = GuiButton(exit_btn, GuiIconText(ICON_CROSS_SMALL, NULL));
    if (state)
      break;
  }

  return 0;
}