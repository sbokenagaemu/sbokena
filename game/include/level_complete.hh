#pragma once

#include "loader.hh"
#include "scene.hh"

using sbokena::loader::Level;

namespace sbokena::game::scene {

class LevelCompleteScene : public Scene {
public:
  LevelCompleteScene()                                      = delete;
  LevelCompleteScene(const LevelCompleteScene &)            = delete;
  LevelCompleteScene &operator=(const LevelCompleteScene &) = delete;
  LevelCompleteScene(LevelCompleteScene &&)                 = delete;
  LevelCompleteScene &operator=(LevelCompleteScene &&)      = delete;
  ~LevelCompleteScene()                                     = default;

  LevelCompleteScene(const Level<Texture> &, u64 moves);

  void         draw() const override;
  UpdateResult update(Input) override;

private:
  Level<Texture> level;
  u64            moves;
};

} // namespace sbokena::game::scene
