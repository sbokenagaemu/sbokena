#pragma once

#include "gameplay.hh"
#include "loader.hh"
#include "scene.hh"

using sbokena::loader::Level;

namespace sbokena::game::scene {

class PauseScene : public Scene {
public:
  PauseScene()                              = delete;
  PauseScene(const PauseScene &)            = delete;
  PauseScene &operator=(const PauseScene &) = delete;
  PauseScene(PauseScene &&)                 = delete;
  PauseScene &operator=(PauseScene &&)      = delete;
  ~PauseScene()                             = default;

  // create a `PauseScene` from a `GameplayScene`
  PauseScene(
    const Level<Texture> &, u64 moves, const GameplayScene &
  );

  void         draw() const override;
  UpdateResult update(Input) override;

private:
  Level<Texture> level;
  u64            moves;
  GameplayScene  gameplay_scene;
};

} // namespace sbokena::game::scene
