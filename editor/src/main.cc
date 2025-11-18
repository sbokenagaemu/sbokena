#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif

#include <Color.hpp>
#include <Window.hpp>
#include <array>
#include <raygui.h>
#include <raylib.h>

#include "types.hh"

using namespace sbokena::types;

// several resolution presets can be implemented via changing the values, while
// retaining the ratio

// taskbar_button_size - 0
// tile_picker_padding - 1
// tile_picker_box_size - 2
// tile_picker_box_padding - 3
// tile_picker_space_inbetween - 4
// view_control_padding - 5
// view_control_button_width - 6
// view_control_button_height - 7
// dropdown_width - 8
// tile_picker_width - 9

int main() {
  std::array<float, 10> sizes = {
    40, 10, 80, 25, 20, 25, 60, 50, 100, (10 * 2 + 25 * 2 + 80 * 2 + 20)};

  float min_width = sizes[9] + 4 * sizes[0] + sizes[8] + 40;
  float min_height = sizes[7] + 4 * sizes[2] + 5 * sizes[4] + 20;

  raylib::Window window(min_width, min_height, "Window",
                        FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  bool exit = false;

  SetWindowMinSize(min_width, min_height);

  const char *options_theme = "Yes;No;IDK";
  int active_theme = 0;
  bool edit_mode_theme = false;

  while (!window.ShouldClose() && !exit) {
    window.BeginDrawing();

    float current_window_width = window.GetWidth();
    float current_window_height = window.GetHeight();

    if (current_window_width < min_width) {
      current_window_width = min_width;
    }

    if (current_window_height < min_height) {
      current_window_height = min_height;
    }

    // DECORATIVE ELEMENTS
    // Clear gray background
    window.ClearBackground(raylib::Color::Gray());

    // tile_picker_vertical_1
    const Rectangle tile_picker_vertical_1 = {0, 0, sizes[1],
                                              current_window_height};
    DrawRectangleRec(tile_picker_vertical_1, raylib::Color::Black());

    // tile_picker_vertical_2
    const Rectangle tile_picker_vertical_2 = {sizes[9] - sizes[1], 0, sizes[1],
                                              current_window_height};
    DrawRectangleRec(tile_picker_vertical_2, raylib::Color::Black());

    // taskbar_line
    const Rectangle taskbar_line = {sizes[9], sizes[0],
                                    current_window_width - sizes[9], 5};
    DrawRectangleRec(taskbar_line, raylib::Color::Black());

    // view_control_line_1
    const Rectangle view_control_line_1 = {sizes[1] + sizes[6],
                                           current_window_height - sizes[7],
                                           sizes[5], sizes[7]};
    DrawRectangleRec(view_control_line_1, raylib::Color::Black());

    // view_control_line_2
    const Rectangle view_control_line_2 = {sizes[1] + sizes[5] + (2 * sizes[6]),
                                           current_window_height - sizes[7],
                                           sizes[5], sizes[7]};
    DrawRectangleRec(view_control_line_2, raylib::Color::Black());

    // BUTTONS
    // Exit button
    const Rectangle exit_button = {current_window_width - sizes[0], 0, sizes[0],
                                   sizes[0]};
    if (GuiButton(exit_button, "Exit")) {
      exit = true;
    }

    // Download button
    const Rectangle download_button = {current_window_width - (2 * sizes[0]), 0,
                                       sizes[0], sizes[0]};
    if (GuiButton(download_button, "Download")) {
      // TODO
    }

    // Import button
    const Rectangle import_button = {current_window_width - (3 * sizes[0]), 0,
                                     sizes[0], sizes[0]};
    if (GuiButton(import_button, "Import")) {
      // TODO
    }

    // Reset button
    const Rectangle reset_button = {current_window_width - (4 * sizes[0]), 0,
                                    sizes[0], sizes[0]};
    if (GuiButton(reset_button, "Reset")) {
      // TODO
    }

    // Theme Dropdown menu
    const Rectangle dropdown_theme = {sizes[9] + 20, 5, sizes[8],
                                      sizes[0] - 10};
    if (GuiDropdownBox(dropdown_theme, options_theme, &active_theme,
                       edit_mode_theme)) {
      edit_mode_theme = !edit_mode_theme;
    }

    /**
    // Undo button (?)
    const Rectangle undo_button = {sizes[9], 0, sizes[0], sizes[0]};
    if (GuiButton(undo_button, "Undo")) {
      // TODO
    }

    // Redo button (?)
    const Rectangle redo_button = {sizes[9] + sizes[0], 0, sizes[0], sizes[0]};
    if (GuiButton(redo_button, "Redo")) {
      // TODO
    }
    */

    // Zoom in button (?)
    const Rectangle zoom_in_button = {
      sizes[1], current_window_height - sizes[7], sizes[6], sizes[7]};
    if (GuiButton(zoom_in_button, "Zoom in")) {
      // TODO
    }

    // Rotate button
    const Rectangle rotate_button = {(sizes[1] + sizes[5]) + sizes[6],
                                     current_window_height - sizes[7], sizes[6],
                                     sizes[7]};
    if (GuiButton(rotate_button, "Rotate")) {
      // TODO
    }

    // Zoom out button (?)
    const Rectangle zoom_out_button = {
      (sizes[1] + 2 * sizes[5]) + (2 * sizes[6]),
      current_window_height - sizes[7], sizes[6], sizes[7]};
    if (GuiButton(zoom_out_button, "Zoom in")) {
      // TODO
    }

    // TILES

    window.EndDrawing();
  }
  return 0;
}