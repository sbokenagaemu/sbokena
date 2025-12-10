//! gameplay scene.

#pragma once

#include <optional>

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

  Position<> min;
  Position<> max;
  usize      width;
  usize      height;

  u64       moves      = 0;
  Direction player_dir = Direction::Down;

  // draw the game state.
  void draw_viewport() const;

  // draw the gameplay HUD.
  void draw_hud() const;

  // get the sprite of a tile at some position.
  // positions not in `tiles` will be shown as a `Wall`.
  Texture tile_sprite_at(Position<>) const;

  // get the sprite of an object at some position.
  // positions not in `objects` will cause an exception.
  std::optional<Texture> obj_sprite_at(Position<>) const;
};

} // namespace sbokena::game::scene
