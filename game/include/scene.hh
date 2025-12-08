//! game scenes and transitions.

#pragma once

#include <memory>
#include <variant>

#include <raylib.h>

namespace sbokena::game::scene {

// keyboard/gamepad-agnostic input.
enum struct Input {
  INPUT_NONE,  // no keys were pressed this frame
  INPUT_UP,    // up arrow / W / d-pad up
  INPUT_DOWN,  // down arrow / S / d-pad down
  INPUT_LEFT,  // left arrow / A / d-pad left
  INPUT_RIGHT, // right arrow / D / d-pad right
  INPUT_MENU,  // escape / start button
  INPUT_BACK,  // backspace / Xbox B button / PS Circle button
  INPUT_ENTER, // enter / Xbox A button / PS Cross button
};

// get an `Input` from a key and button query.
Input from_queues(
  decltype(GetKeyPressed())           key,
  decltype(GetGamepadButtonPressed()) btn
);

// the `Scene` updated normally.
struct UpdateOk;

// the `Scene` wants to transition to another one.
// this transition will be handled externally.
struct UpdateTransition;

// the `Scene` wants to close the game.
struct UpdateClose;

// the result of calling `Scene::update`.
using UpdateResult =
  std::variant<UpdateOk, UpdateTransition, UpdateClose>;

// a game scene.
struct Scene {
  Scene()          = default;
  virtual ~Scene() = default;

  // draw the current scene state to the `raylib` window.
  //
  // this method will be called once per frame.
  //
  // `BeginDrawing` and `EndDrawing` are called externally.
  // the implementation should still call `ClearBackground` if needed.
  //
  // this method is mostly used to draw non-GUI elements.
  virtual void draw() const = 0;

  // update the current scene state with a received input.
  //
  // this method will be called once per frame, after `draw`.
  //
  // this method may also draw to the screen (mainly with `raygui`).
  //
  // this method is mostly used to react to player input, and to draw
  // GUI elements.
  virtual UpdateResult update(Input) = 0;
};

// ===== impls for `UpdateResult` =====

struct UpdateOk {};

struct UpdateTransition {
  std::unique_ptr<Scene> next;
};

struct UpdateClose {};

} // namespace sbokena::game::scene
