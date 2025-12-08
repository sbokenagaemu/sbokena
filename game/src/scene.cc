#include "scene.hh"

#include <raylib.h>

namespace sbokena::game::scene {

Input from_queues(
  decltype(GetKeyPressed())           key,
  decltype(GetGamepadButtonPressed()) btn
) {
  switch (btn) {
  case GAMEPAD_BUTTON_LEFT_FACE_UP:
    return Input::INPUT_UP;
  case GAMEPAD_BUTTON_LEFT_FACE_DOWN:
    return Input::INPUT_DOWN;
  case GAMEPAD_BUTTON_LEFT_FACE_LEFT:
    return Input::INPUT_LEFT;
  case GAMEPAD_BUTTON_LEFT_FACE_RIGHT:
    return Input::INPUT_RIGHT;
  case GAMEPAD_BUTTON_MIDDLE_RIGHT:
    return Input::INPUT_MENU;
  case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT:
    return Input::INPUT_BACK;
  case GAMEPAD_BUTTON_RIGHT_FACE_DOWN:
    return Input::INPUT_ENTER;
  default:
    switch (key) {
    case KEY_W:
      return Input::INPUT_UP;
    case KEY_S:
      return Input::INPUT_DOWN;
    case KEY_A:
      return Input::INPUT_LEFT;
    case KEY_D:
      return Input::INPUT_RIGHT;
    case KEY_ESCAPE:
      return Input::INPUT_MENU;
    case KEY_BACKSPACE:
      return Input::INPUT_BACK;
    case KEY_ENTER:
      return Input::INPUT_ENTER;
    default:
      return Input::INPUT_NONE;
    }
  }
}

} // namespace sbokena::game::scene
