//! gameplay scene.

#pragma once

#include <raylib.h>

#include "loader.hh"
#include "scene.hh"
#include "state.hh"

using sbokena::game::state::State;
using namespace sbokena::loader;

namespace sbokena::game::scene {

class GameplayScene : public Scene {
public:
  GameplayScene()                                 = delete;
  GameplayScene(const GameplayScene &)            = delete;
  GameplayScene &operator=(const GameplayScene &) = delete;
  GameplayScene(GameplayScene &&)                 = delete;
  GameplayScene &operator=(GameplayScene &&)      = delete;
  ~GameplayScene()                                = default;

  // create a `GameplayScene` from a `Level`.
  GameplayScene(const Level<Texture> &);

  void         draw() const override;
  UpdateResult update(Input) override;

private:
  State          state;
  Theme<Texture> theme;
  u64            moves = 0;

  // draw the game state.
  void draw_viewport() const;

  // draw the gameplay HUD.
  void draw_hud() const;
};

} // namespace sbokena::game::scene
