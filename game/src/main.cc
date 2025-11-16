#include <raylib.h>

int main() {
#ifdef MACOS_COCOA_BACKEND
  // macOS OpenGL 4.1 hints (GLFW handled internally by raylib)
#endif

  // Initialize window
  InitWindow(800, 600, "Sbokena");
  SetTargetFPS(60);

  // Main loop
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("You are hacked!\n- Sbokena", 200, 200, 20, DARKGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}