#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif

#include <Color.hpp>
#include <Window.hpp>
#include <raygui.h>
#include <raylib.h>

#include "editor_level.hh"

using namespace sbokena::types;
using namespace sbokena::editor::level;

// for testing purposes
#include <iostream>

// several resolution presets can be implemented via
// changing the values, while retaining the ratio
constexpr u32 width                       = 1800;
constexpr u32 height                      = 1000;
constexpr u32 taskbar_button_size         = 40;
constexpr u32 tile_picker_padding         = 10;
constexpr u32 tile_picker_box_size        = 160;
constexpr u32 tile_picker_box_padding     = 25;
constexpr u32 tile_picker_space_inbetween = 50;
constexpr u32 view_control_padding        = 30;
constexpr u32 view_control_button_width   = 120;
constexpr u32 view_control_button_height  = 50;

int main() {
  Level         level_ = Level("default");
  raylib::Color color_;

  std::array<float, 10> sizes = {
    40,
    10,
    80,
    25,
    20,
    25,
    60,
    50,
    100,
    (10 * 2 + 25 * 2 + 80 * 2 + 20)
  };

  float min_width  = sizes[9] + 4 * sizes[0] + sizes[8] + 40;
  float min_height = sizes[7] + 4 * sizes[2] + 5 * sizes[4] + 20;

  raylib::Window window(
    min_width,
    min_height,
    "Window",
    FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE
  );
  bool exit = false;

  SetWindowMinSize(min_width, min_height);

  const char          *options_theme   = "Yes;No;IDK";
  int                  active_theme    = 0;
  bool                 edit_mode_theme = false;
  std::array<float, 4> zoom_values     = {0.25, 0.5, 1, 2};
  float                current_zoom    = 2;

  while (!window.ShouldClose() && !exit) {
    window.BeginDrawing();

    // Clear gray background
    window.ClearBackground(raylib::Color::LightGray());

    // resizes the window to be at least it's minimum
    float current_window_width  = window.GetWidth();
    float current_window_height = window.GetHeight();

    if (current_window_width < min_width)
      current_window_width = min_width;

    if (current_window_height < min_height)
      current_window_height = min_height;

    // GRID
    float x_offset = 0;
    float y_offset = 0;
    float grid_size = 64 * zoom_values[current_zoom];
    for (u32 y = 0; y < 31; y++) {
      for (u32 x = 0; x < 31; x++) {
        const Rectangle tile = {x_offset + grid_size * x,
                                y_offset + grid_size * y, grid_size, grid_size};
        if ((x + y) % 2) {
          color_ = raylib::Color::Black();
        } else {
          color_ = raylib::Color::White();
        }
        DrawRectangleRec(tile, color_);
      }
    }

    std::cout << current_zoom << std::endl;

    // DECORATIVE ELEMENTS
    // tile_picker
    const Rectangle tile_picker = {
      0, 0, sizes[9], current_window_height
    };
    DrawRectangleRec(tile_picker, raylib::Color::Gray());

    // tile_picker_vertical_1
    const Rectangle tile_picker_vertical_1 = {
      0, 0, tile_picker_padding, height
    };
    DrawRectangleRec(tile_picker_vertical_1, raylib::Color::Black());

    // tile_picker_vertical_2
    const Rectangle tile_picker_vertical_2 = {
      tile_picker_width - tile_picker_padding,
      0,
      tile_picker_padding,
      height
    };
    DrawRectangleRec(tile_picker_vertical_2, raylib::Color::Black());

    // taskbar
    const Rectangle taskbar = {
      sizes[9], 0, current_window_width - sizes[9], sizes[0]
    };
    DrawRectangleRec(taskbar, raylib::Color::Gray());

    // taskbar_line
    const Rectangle taskbar_line = {
      tile_picker_width,
      taskbar_button_size,
      width - tile_picker_width,
      5
    };
    DrawRectangleRec(taskbar_line, raylib::Color::Black());

    // view_control_line_1
    const Rectangle view_control_line_1 = {
      tile_picker_padding + view_control_button_width,
      height - view_control_button_height,
      view_control_padding,
      view_control_button_height
    };
    DrawRectangleRec(view_control_line_1, raylib::Color::Black());

    // view_control_line_2
    const Rectangle view_control_line_2 = {
      tile_picker_padding + view_control_padding
        + (2 * view_control_button_width),
      height - view_control_button_height,
      view_control_padding,
      view_control_button_height
    };
    DrawRectangleRec(view_control_line_2, raylib::Color::Black());

    // BUTTONS
    // Exit button
    const Rectangle exit_button = {
      width - taskbar_button_size,
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(exit_button, "Exit"))
      exit = true;

    // Download button
    const Rectangle download_button = {
      width - (2 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(download_button, "Download")) {
      // TODO
    }

    // Import button
    const Rectangle import_button = {
      width - (3 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(import_button, "Import")) {
      // TODO
    }

    // Reset button
    const Rectangle reset_button = {
      width - (4 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(reset_button, "Reset")) {
      // TODO
    }

    // Undo button (?)
    const Rectangle undo_button = {
      tile_picker_width, 0, taskbar_button_size, taskbar_button_size
    };
    if (GuiButton(undo_button, "Undo")) {
      // TODO
    }

    // Redo button (?)
    const Rectangle redo_button = {
      tile_picker_width + taskbar_button_size,
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(redo_button, "Redo")) {
      // TODO
    }

    // Zoom in button (?)
    const Rectangle zoom_in_button = {
      tile_picker_padding,
      height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(zoom_in_button, "Zoom in")) {
      if (current_zoom < 3)
        current_zoom += 1;
    }

    // Rotate button
    const Rectangle rotate_button = {
      (tile_picker_padding + view_control_padding)
        + view_control_button_width,
      height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(rotate_button, "Rotate")) {
      // TODO
    }

    // Zoom out button (?)
    const Rectangle zoom_out_button = {
      (sizes[1] + 2 * sizes[5]) + (2 * sizes[6]),
      current_window_height - sizes[7],
      sizes[6],
      sizes[7]
    };
    if (GuiButton(zoom_out_button, "Zoom out")) {
      if (current_zoom != 0)
        current_zoom -= 1;
    }

    // TILES

    window.EndDrawing();
  }
  return 0;
}
