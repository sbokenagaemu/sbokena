#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "Color.hpp"
#endif

#include <Window.hpp>
#include <raygui.h>
#include <raylib.h>
#include <types.hh>

using namespace sbokena::types;

// several resolution presets can be implemented via changing the values, while
// retaining the ratio
constexpr u32 width = 1800;
constexpr u32 height = 1000;
constexpr u32 taskbar_button_size = 40;
constexpr u32 tile_picker_padding = 10;
constexpr u32 tile_picker_box_size = 160;
constexpr u32 tile_picker_box_padding = 25;
constexpr u32 tile_picker_space_inbetween = 50;
constexpr u32 view_control_padding = 30;
constexpr u32 view_control_button_width = 120;
constexpr u32 view_control_button_height = 50;

int main() {
  raylib::Window window(width, height, "Window");
  window.SetTargetFPS(60);
  bool exit = false;

  // The width of the tile picker section
  const f32 tile_picker_width =
    ((2 * tile_picker_space_inbetween) + (2 * tile_picker_box_size) +
     (2 * tile_picker_padding));

  while (!window.ShouldClose() && !exit) {
    window.BeginDrawing();

    // DECORATIVE ELEMENTS
    // Clear gray background
    window.ClearBackground(raylib::Color::Gray());

    // tile_picker_vertical_1
    const Rectangle tile_picker_vertical_1 = {0, 0, tile_picker_padding,
                                              height};
    DrawRectangleRec(tile_picker_vertical_1, raylib::Color::Black());

    // tile_picker_vertical_2
    const Rectangle tile_picker_vertical_2 = {
      tile_picker_width - tile_picker_padding, 0, tile_picker_padding, height};
    DrawRectangleRec(tile_picker_vertical_2, raylib::Color::Black());

    // taskbar_line
    const Rectangle taskbar_line = {tile_picker_width, taskbar_button_size,
                                    width - tile_picker_width, 5};
    DrawRectangleRec(taskbar_line, raylib::Color::Black());

    // view_control_line_1
    const Rectangle view_control_line_1 = {
      tile_picker_padding + view_control_button_width,
      height - view_control_button_height, view_control_padding,
      view_control_button_height};
    DrawRectangleRec(view_control_line_1, raylib::Color::Black());

    // view_control_line_2
    const Rectangle view_control_line_2 = {
      tile_picker_padding + view_control_padding +
        (2 * view_control_button_width),
      height - view_control_button_height, view_control_padding,
      view_control_button_height};
    DrawRectangleRec(view_control_line_2, raylib::Color::Black());

    // BUTTONS
    // Exit button
    const Rectangle exit_button = {width - taskbar_button_size, 0,
                                   taskbar_button_size, taskbar_button_size};
    if (GuiButton(exit_button, "Exit")) {
      exit = true;
    }

    // Download button
    const Rectangle download_button = {width - (2 * taskbar_button_size), 0,
                                       taskbar_button_size,
                                       taskbar_button_size};
    if (GuiButton(download_button, "Download")) {
      // TODO
    }

    // Import button
    const Rectangle import_button = {width - (3 * taskbar_button_size), 0,
                                     taskbar_button_size, taskbar_button_size};
    if (GuiButton(import_button, "Import")) {
      // TODO
    }

    // Reset button
    const Rectangle reset_button = {width - (4 * taskbar_button_size), 0,
                                    taskbar_button_size, taskbar_button_size};
    if (GuiButton(reset_button, "Reset")) {
      // TODO
    }

    // Undo button (?)
    const Rectangle undo_button = {tile_picker_width, 0, taskbar_button_size,
                                   taskbar_button_size};
    if (GuiButton(undo_button, "Undo")) {
      // TODO
    }

    // Redo button (?)
    const Rectangle redo_button = {tile_picker_width + taskbar_button_size, 0,
                                   taskbar_button_size, taskbar_button_size};
    if (GuiButton(redo_button, "Redo")) {
      // TODO
    }

    // Zoom in button (?)
    const Rectangle zoom_in_button = {
      tile_picker_padding, height - view_control_button_height,
      view_control_button_width, view_control_button_height};
    if (GuiButton(zoom_in_button, "Zoom in")) {
      // TODO
    }

    // Rotate button
    const Rectangle rotate_button = {
      (tile_picker_padding + view_control_padding) + view_control_button_width,
      height - view_control_button_height, view_control_button_width,
      view_control_button_height};
    if (GuiButton(rotate_button, "Rotate")) {
      // TODO
    }

    // Zoom out button (?)
    const Rectangle zoom_out_button = {
      (tile_picker_padding + 2 * view_control_padding) +
        (2 * view_control_button_width),
      height - view_control_button_height, view_control_button_width,
      view_control_button_height};
    if (GuiButton(zoom_out_button, "Zoom in")) {
      // TODO
    }

    // TILES

    window.EndDrawing();
  }
}