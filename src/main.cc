#include <print>
#include <string>

#include "Color.hpp"
#include "Font.hpp"
#include "Window.hpp"
#include "raylib.h"

int main() {
  std::println("hello, world!");

  const int window_width = 800;
  const int window_height = 600;
  const std::string window_title = "main";

  raylib::Window window = {
    window_width,
    window_height,
    window_title,
  };

  window.SetTargetFPS(60);

  const raylib::Font font;

  while (!window.ShouldClose()) {
    window.BeginDrawing();
    window.ClearBackground(raylib::Color::RayWhite());

    const std::string msg = "hello, raylib!";
    const raylib::Vector2 size = font.MeasureText(msg, 20, 1);
    const raylib::Vector2 pos = (window.GetSize() - size) / 2;
    font.DrawText(msg, pos, 20, 1, raylib::Color::DarkGray());

    window.EndDrawing();
  }

  return 0;
}
