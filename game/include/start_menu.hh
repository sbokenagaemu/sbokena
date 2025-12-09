//! start menu scene.

#pragma once

#include <optional>

#include "scene.hh"
#include "types.hh"

using namespace sbokena::types;

namespace sbokena::game::scene {

class StartMenuScene : public Scene {
public:
  StartMenuScene()                                  = default;
  StartMenuScene(const StartMenuScene &)            = delete;
  StartMenuScene &operator=(const StartMenuScene &) = delete;
  StartMenuScene(StartMenuScene &&)                 = delete;
  StartMenuScene &operator=(StartMenuScene &&)      = delete;
  ~StartMenuScene()                                 = default;

  void         draw() const override;
  UpdateResult update(Input) override;

private:
  std::optional<f64> show_failed_load = std::nullopt;
};

}; // namespace sbokena::game::scene
