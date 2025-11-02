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

int main() {
  raylib::Window window(width, height, "Window");
  window.SetTargetFPS(60);
  bool exit = false;

  while (!window.ShouldClose() && !exit) {
    window.BeginDrawing();

    // Clear background (testing purposes)
    window.ClearBackground(GRAY);

    // BUTTONS

    // TILES

    window.EndDrawing();
  }
}