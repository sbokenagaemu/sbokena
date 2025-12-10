#include <cstdlib>
#include <memory>
#include <string_view>

#include <nfd.h>
#include <raylib.h>
#include <raymath.h>

#include "level_complete.hh"
#include "pause.hh"
#include "scene.hh"
#include "start_menu.hh"
#include "types.hh"
#include "utils.hh"

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif
#include <raygui.h>

using namespace std::string_view_literals;

using namespace sbokena::types;
using namespace sbokena::utils;
using namespace sbokena::game::scene;

constexpr std::string_view TITLE = "sbokena"sv;

constexpr u32 SCREEN_W_DEFAULT    = 800;
constexpr u32 SCREEN_H_DEFAULT    = 600;
constexpr u32 SCREEN_W_MAX        = 3840;
constexpr u32 SCREEN_H_MAX        = 2160;
constexpr u32 RAYGUI_TEXT_SIZE    = 30;
constexpr u32 RAYGUI_TEXT_SPACING = 4;

i32 main() {
  // ===== initialize raylib =====

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(SCREEN_W_DEFAULT, SCREEN_H_DEFAULT, TITLE.data());
  Deferred _ {CloseWindow};

  SetWindowMinSize(SCREEN_W_DEFAULT, SCREEN_H_DEFAULT);
  SetWindowMaxSize(SCREEN_W_MAX, SCREEN_H_MAX);
  SetExitKey(KEY_NULL);

  // ===== initialize raygui =====

  GuiSetStyle(DEFAULT, TEXT_SIZE, RAYGUI_TEXT_SIZE);
  GuiSetStyle(DEFAULT, TEXT_SPACING, RAYGUI_TEXT_SPACING);

  // ===== initialize nfd =====

  NFD::Guard _nfd;

  // ===== main loop =====

  std::unique_ptr<Scene> cur {new StartMenuScene};
  std::unique_ptr<Scene> next {nullptr};

  while (!WindowShouldClose()) {
    // ===== draw phase =====

    BeginDrawing();
    Deferred _ {EndDrawing};
    cur->draw();

    // ===== input phase =====

    const auto  key   = GetKeyPressed();
    const auto  btn   = GetGamepadButtonPressed();
    const Input input = from_queues(key, btn);

    // ===== update phase =====

    UpdateResult res = cur->update(input);
    switch (res.index()) {
    // scene doesn't want to transition
    case index_of<UpdateResult, UpdateOk>():
      break;

    // scene wants to transition, do so after drawing
    // store the next scene into a temporary pointer
    case index_of<UpdateResult, UpdateTransition>(): {
      UpdateTransition &trans = std::get<UpdateTransition>(res);
      next.swap(trans.next);
      break;
    }

    // scene wants to close the game, just return from main
    case index_of<UpdateResult, UpdateClose>():
      return EXIT_SUCCESS;
    }

    // ===== transition phase =====

    if (next)
      cur = std::move(next);
  }
}
