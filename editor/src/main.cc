#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "Window.hpp"
#endif

#include <nlohmann/json.hpp>
#include <raygui.h>
#include <raylib.h>

// several resolution presets can be implemented via changing the values, while
// retaining the ratio
#define width 1800
#define height 1000
#define taskbar_button_size 40
#define tile_picker_padding 10
#define tile_picker_box_size 160
#define tile_picker_space_inbetween 5
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

    // Clear background (testing purposes)
    window.ClearBackground(GRAY);

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

    // TILES

    window.EndDrawing();
  }
}