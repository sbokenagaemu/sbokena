#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif

#include <Color.hpp>
#include <Window.hpp>
#include <raygui.h>
#include <raylib.h>

#include "editor_level.hh"
#include "grid.hh"

using namespace sbokena::types;
using namespace sbokena::editor::level;

// for testing purposes
#include <iostream>

constexpr u32 taskbar_button_size         = 40;
constexpr u32 tile_picker_padding         = 10;
constexpr u32 tile_picker_box_size        = 80;
constexpr u32 tile_picker_box_padding     = 25;
constexpr u32 tile_picker_space_inbetween = 20;
constexpr u32 view_control_padding        = 25;
constexpr u32 view_control_button_width   = 60;
constexpr u32 view_control_button_height  = 50;
constexpr u32 dropdown_width              = 100;
constexpr u32 grid_view_min_height        = 45;
// also the minimum width of the grid-view
constexpr u32 tile_picker_width = 250;
constexpr f32 min_width         = 550;
constexpr f32 min_height        = 470;

int main() {
  Level         level_ = Level("default");
  raylib::Color color_;

  raylib::Window window(
    min_width,
    min_height,
    "Window",
    FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE
  );
  bool exit = false;

  SetWindowMinSize(min_width, min_height);

  const char     *options_theme     = "Theme #1;Theme #2;Theme #3";
  int             active_theme      = 0;
  bool            edit_mode_theme   = false;
  f32             current_tile_size = 64;
  raylib::Vector2 mouse_position;
  raylib::Vector2 mouse_start_position;
  raylib::Vector2 grid_offset              = {0, 0};
  raylib::Vector2 selected_tile_position   = {0, 0};
  raylib::Vector2 selected_grid_tile_index = {0, 0};
  bool            is_selection_shown       = false;

  while (!window.ShouldClose() && !exit) {
    window.BeginDrawing();
    mouse_position = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
      mouse_start_position = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
      raylib::Vector2 difference =
        Vector2Subtract(mouse_position, mouse_start_position);
      grid_offset += difference;
      selected_tile_position += difference;
      mouse_start_position = GetMousePosition();
    }

    // Clear gray background
    window.ClearBackground(raylib::Color::LightGray());

    // resizes the window to be at least it's minimum
    f32 current_window_width  = window.GetWidth();
    f32 current_window_height = window.GetHeight();

    if (current_window_width < min_width)
      current_window_width = min_width;

    if (current_window_height < min_height)
      current_window_height = min_height;

    // GRID
    for (u32 y = 0; y < 31; y++) {
      for (u32 x = 0; x < 31; x++) {
        const Rectangle tile = {
          grid_offset.GetX() + current_tile_size * x,
          grid_offset.GetY() + current_tile_size * y,
          current_tile_size,
          current_tile_size
        };
        if ((x + y) % 2)
          color_ = raylib::Color::Black();
        else
          color_ = raylib::Color::White();
        DrawRectangleRec(tile, color_);
      }
    }

    // tile selection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      // checks whether:
      // 1) the mouse is inside the window's grid part
      // 2) the mouse is inside the actual grid
      if (is_inside(
            mouse_position,
            {tile_picker_width, grid_view_min_height},
            {current_window_width, current_window_height}
          )
          && is_inside(
            mouse_position,
            grid_offset,
            grid_end(grid_offset, current_tile_size)
          )) {
        selected_grid_tile_index =
          tile_index(mouse_position, grid_offset, current_tile_size);

        std::cout << "yes" << std::endl;

        selected_tile_position = grid_offset.Add(
          selected_grid_tile_index.Scale(current_tile_size)
        );

        is_selection_shown = true;
      }
    }

    // deselecting the tile
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
      is_selection_shown = false;

    // showing the selection
    if (is_selection_shown) {
      const Rectangle selection = {
        selected_tile_position.GetX(),
        selected_tile_position.GetY(),
        current_tile_size,
        current_tile_size
      };
      DrawRectangleRec(selection, Fade(raylib::Color::Gray(), 0.5));
    }

    // DECORATIVE ELEMENTS
    // tile_picker
    const Rectangle tile_picker = {
      0, 0, tile_picker_width, current_window_height
    };
    DrawRectangleRec(tile_picker, raylib::Color::Gray());

    // tile_picker_vertical_1
    const Rectangle tile_picker_vertical_1 = {
      0, 0, tile_picker_padding, current_window_height
    };
    DrawRectangleRec(tile_picker_vertical_1, raylib::Color::Black());

    // tile_picker_vertical_2
    const Rectangle tile_picker_vertical_2 = {
      tile_picker_width - tile_picker_padding,
      0,
      tile_picker_padding,
      current_window_height
    };
    DrawRectangleRec(tile_picker_vertical_2, raylib::Color::Black());

    // taskbar
    const Rectangle taskbar = {
      tile_picker_width,
      0,
      current_window_width - tile_picker_width,
      taskbar_button_size
    };
    DrawRectangleRec(taskbar, raylib::Color::Gray());

    // taskbar_line
    const Rectangle taskbar_line = {
      tile_picker_width,
      taskbar_button_size,
      current_window_width - tile_picker_width,
      5
    };
    DrawRectangleRec(taskbar_line, raylib::Color::Black());

    // view_control_line_1
    const Rectangle view_control_line_1 = {
      tile_picker_padding + view_control_button_width,
      current_window_height - view_control_button_height,
      view_control_padding,
      view_control_button_height
    };
    DrawRectangleRec(view_control_line_1, raylib::Color::Black());

    // view_control_line_2
    const Rectangle view_control_line_2 = {
      tile_picker_padding + view_control_padding
        + (2 * view_control_button_width),
      current_window_height - view_control_button_height,
      view_control_padding,
      view_control_button_height
    };
    DrawRectangleRec(view_control_line_2, raylib::Color::Black());

    // BUTTONS
    // Exit button
    const Rectangle exit_button = {
      current_window_width - taskbar_button_size,
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(exit_button, "Exit"))
      exit = true;

    // Download button
    const Rectangle download_button = {
      current_window_width - (2 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(download_button, "Download")) {
      // TODO
    }

    // Import button
    const Rectangle import_button = {
      current_window_width - (3 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(import_button, "Import")) {
      // TODO
    }

    // Reset button
    const Rectangle reset_button = {
      current_window_width - (4 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(reset_button, "Reset")) {
      // TODO
    }

    // Theme Dropdown menu
    const Rectangle dropdown_theme = {
      tile_picker_width + 20,
      5,
      dropdown_width,
      taskbar_button_size - 10
    };
    if (GuiDropdownBox(
          dropdown_theme,
          options_theme,
          &active_theme,
          edit_mode_theme
        )) {
      edit_mode_theme = !edit_mode_theme;
    }

    /**
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
    */

    // Zoom in button (?)
    const Rectangle zoom_in_button = {
      tile_picker_padding,
      current_window_height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(zoom_in_button, "Zoom in")) {
      if (current_tile_size <= 256) {
        current_tile_size *= 2;
        selected_tile_position = grid_offset.Add(
          selected_grid_tile_index.Scale(current_tile_size)
        );
      }
    }

    // Rotate button
    const Rectangle rotate_button = {
      (tile_picker_padding + view_control_padding)
        + view_control_button_width,
      current_window_height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(rotate_button, "Rotate")) {
      // TODO
    }

    // Zoom out button (?)
    const Rectangle zoom_out_button = {
      (tile_picker_padding + 2 * view_control_padding)
        + (2 * view_control_button_width),
      current_window_height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(zoom_out_button, "Zoom out")) {
      if (current_tile_size >= 32) {
        current_tile_size /= 2;
        selected_tile_position = grid_offset.Add(
          selected_grid_tile_index.Scale(current_tile_size)
        );
      }
    }

    // TILES

    window.EndDrawing();
  }
  return 0;
}
