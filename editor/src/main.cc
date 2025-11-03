#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif

#include <Window.hpp>
#include <raygui.h>
#include <raylib.h>
#include <types.hh>

using namespace sbokena::types;

// several resolution presets can be implemented via changing the values, while
// retaining the ratio
#define width 1800
#define height 1000
#define taskbar_button_size 40
#define tile_picker_padding 10
#define tile_picker_box_size 160
#define tile_picker_box_padding 25
#define tile_picker_space_inbetween 50
#define view_control_padding 30
#define view_control_button_width 120
#define view_control_button_height 50

int main() {
  raylib::Window window(width, height, "Window");
  window.SetTargetFPS(60);
  bool exit = false;

  // The width of the tile picker section
  float tile_picker_width =
    ((2 * tile_picker_space_inbetween) + (2 * tile_picker_box_size) +
     (2 * tile_picker_padding));

  while (!window.ShouldClose() && !exit) {
    window.BeginDrawing();

    // DECORATIVE ELEMENTS
    // Clear gray background
    window.ClearBackground(GRAY);

    // tile_picker_vertical_1
    DrawRectangleRec({0, 0, tile_picker_padding, height}, BLACK);

    // tile_picker_vertical_2
    DrawRectangleRec(
      {tile_picker_width - tile_picker_padding, 0, tile_picker_padding, height},
      BLACK);

    // taskbar_line
    DrawRectangleRec(
      {tile_picker_width, taskbar_button_size, width - tile_picker_width, 5},
      BLACK);

    // view_control_line_1
    DrawRectangleRec({tile_picker_padding + view_control_button_width,
                      height - view_control_button_height, view_control_padding,
                      view_control_button_height},
                     BLACK);

    // view_control_line_2
    DrawRectangleRec({tile_picker_padding + view_control_padding +
                        (2 * view_control_button_width),
                      height - view_control_button_height, view_control_padding,
                      view_control_button_height},
                     BLACK);

    // BUTTONS
    // Exit button
    if (GuiButton({width - taskbar_button_size, 0, taskbar_button_size,
                   taskbar_button_size},
                  "Exit")) {
      exit = true;
    }

    // Download button
    if (GuiButton({width - (2 * taskbar_button_size), 0, taskbar_button_size,
                   taskbar_button_size},
                  "Download")) {
      // TODO
    }

    // Import button
    if (GuiButton({width - (3 * taskbar_button_size), 0, taskbar_button_size,
                   taskbar_button_size},
                  "Import")) {
      // TODO
    }

    // Reset button
    if (GuiButton({width - (4 * taskbar_button_size), 0, taskbar_button_size,
                   taskbar_button_size},
                  "Reset")) {
      // TODO
    }

    // Undo button (?)
    if (GuiButton(
          {tile_picker_width, 0, taskbar_button_size, taskbar_button_size},
          "Undo")) {
      // TODO
    }

    // Redo button (?)
    if (GuiButton({tile_picker_width + taskbar_button_size, 0,
                   taskbar_button_size, taskbar_button_size},
                  "Redo")) {
      // TODO
    }

    // Zoom in button (?)
    if (GuiButton({tile_picker_padding, height - view_control_button_height,
                   view_control_button_width, view_control_button_height},
                  "Zoom in")) {
      // TODO
    }

    // Rotate button
    if (GuiButton({(tile_picker_padding + view_control_padding) +
                     view_control_button_width,
                   height - view_control_button_height,
                   view_control_button_width, view_control_button_height},
                  "Rotate")) {
      // TODO
    }

    // Zoom out button (?)
    if (GuiButton({(tile_picker_padding + 2 * view_control_padding) +
                     (2 * view_control_button_width),
                   height - view_control_button_height,
                   view_control_button_width, view_control_button_height},
                  "Zoom in")) {
      // TODO
    }

    // TILES

    window.EndDrawing();
  }
}