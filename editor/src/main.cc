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

int main() {
  raylib::Window window(width, height, "Window");
  window.SetTargetFPS(60);
  bool exit = false;

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

    // TILES

    window.EndDrawing();
  }
}