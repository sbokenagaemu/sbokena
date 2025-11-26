#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif

#include <print>
#include <string>

#include <nfd.h>
#include <raygui.h>
#include <raylib.h>
#include <raymath.h>

#include "types.hh"
#include "utils.hh"

using namespace sbokena::types;
using namespace sbokena::utils;

// initial window parameters
constexpr usize       INIT_WIDTH  = 800;
constexpr usize       INIT_HEIGHT = 600;
constexpr std::string INIT_TITLE  = "sbokena";

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(INIT_WIDTH, INIT_HEIGHT, INIT_TITLE.c_str());
  SetWindowMinSize(INIT_WIDTH, INIT_HEIGHT);
  Deferred _ {CloseWindow};

  NFD::Guard nfd;

  GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
  GuiSetStyle(DEFAULT, TEXT_SPACING, 4);

  // show file open failed message
  bool show_file_open_failed = false;

  while (!WindowShouldClose()) {
    BeginDrawing();
    Deferred _ {EndDrawing};

    const usize window_width  = GetScreenWidth();
    const usize window_height = GetScreenHeight();
    Vector2     window_size   = {
      static_cast<f32>(window_width), static_cast<f32>(window_height)
    };

    // size and position of title
    Vector2 title_size =
      MeasureTextEx(GuiGetFont(), "sbokena", 60, 10);
    Vector2 title_pos = {
      window_width / 2 - title_size.x / 2,
      static_cast<f32>(window_height / 5)
    };

    // size and position of the view containing buttons
    Vector2 view_size = {180, 300};
    Vector2 view_pos =
      (window_size - view_size) / 2 + Vector2 {0, 100};

    // size of buttons
    Vector2 btn_size = {180, 90};

    // size of file open failed message box
    Vector2 message_size = {300, 60};

    Rectangle exit_btn = {
      .x      = static_cast<f32>(window_width) - 20,
      .y      = 0,
      .width  = 20,
      .height = 20,
    };
    Rectangle start_btn = {
      .x      = view_pos.x,
      .y      = view_pos.y,
      .width  = btn_size.x,
      .height = btn_size.y,
    };
    Rectangle custom_btn = {
      .x      = view_pos.x,
      .y      = view_pos.y + btn_size.y + 10,
      .width  = btn_size.x,
      .height = btn_size.y,
    };
    Rectangle setting_btn = {
      .x      = view_pos.x,
      .y      = view_pos.y + 2 * btn_size.y + 20,
      .width  = btn_size.x,
      .height = btn_size.y,
    };
    Rectangle message_box = {
      .x      = (window_width - message_size.x) / 2,
      .y      = window_height - message_size.y,
      .width  = message_size.x,
      .height = message_size.y,
    };

    ClearBackground(VIOLET);

    // title
    DrawTextEx(GuiGetFont(), "sbokena", title_pos, 60, 10, BLUE);

    // start button
    GuiButton(start_btn, "play");

    // custom button
    // if pressed, file dialog open
    // if no file is chosen, display a message for 3.5s
    f64 start_message;
    if (GuiButton(custom_btn, "custom")) {
      const auto path = open_file_dialog().value_or("");
      if (path.empty()) {
        show_file_open_failed = true;
        // record time of first message display
        start_message = GetTime();
      } else {
        std::println("{}", path.c_str());
      }
    }

    // display file open failed message
    if (show_file_open_failed) {
      GuiDrawText(
        "Fail to load file", message_box, TEXT_ALIGN_CENTER, RED
      );
      if (GetTime() - start_message >= 3.5)
        show_file_open_failed = false;
    }

    // setting button
    GuiButton(setting_btn, "settings");

    // exit button
    bool state =
      GuiButton(exit_btn, GuiIconText(ICON_CROSS_SMALL, NULL));
    if (state)
      break;
  }

  return 0;
}
