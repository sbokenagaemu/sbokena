//! start menu scene.

#pragma once

#include "scene.hh"

namespace sbokena::game::scene {

struct StartMenuScene : Scene {
  StartMenuScene()                                  = default;
  StartMenuScene(const StartMenuScene &)            = delete;
  StartMenuScene &operator=(const StartMenuScene &) = delete;
  StartMenuScene(StartMenuScene &&)                 = delete;
  StartMenuScene &operator=(StartMenuScene &&)      = delete;
  ~StartMenuScene()                                 = default;

  void         draw() const override;
  UpdateResult update(Input) override;
};

}; // namespace sbokena::game::scene
