#include "raylib.h"
#include <print>

int main() {
  std::println("hello, world!");
  std::println("raylib version {}", RAYLIB_VERSION);

  const int window_width = 800;
  const int window_height = 600;
  const char *window_title = "main";

  InitWindow(window_width, window_height, window_title);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // clang-format off
    const char *msg = "hello, raylib!";
    const Vector2 size = MeasureTextEx(GetFontDefault(), msg, 20, 0);
    DrawText(
      msg,
      (window_width - size.x) / 2,
      (window_height - size.y) / 2, 20,
      DARKGRAY
    );
    // clang-format on

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
