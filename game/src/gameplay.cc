#include "gameplay.hh"

#include <algorithm>
#include <format>
#include <memory>
#include <string_view>
#include <variant>

#include <raygui.h>
#include <raylib.h>
#include <raymath.h>

#include "direction.hh"
#include "level.hh"
#include "level_complete.hh"
#include "loader.hh"
#include "pause.hh"
#include "position.hh"
#include "scene.hh"
#include "state.hh"
#include "utils.hh"

using namespace std::string_view_literals;

using namespace sbokena::utils;
using namespace sbokena::position;
using sbokena::direction::Direction;
using sbokena::game::state::StepResult;
using sbokena::loader::Level;

namespace sbokena::game::scene {

GameplayScene::GameplayScene(const Level<Texture> &level)
  : state {level},
    level {level},
    min {POS_MAX<>},
    max {POS_MIN<>} {
  for (const auto &[pos, _] : state.inner().tiles) {
    min = {
      .x = std::min(min.x, pos.x),
      .y = std::min(min.y, pos.y),
    };
    max = {
      .x = std::max(max.x, pos.x),
      .y = std::max(max.y, pos.y),
    };
  }

  // `min - max` counts the gaps between tiles.
  width  = (max - min).x + 1;
  height = (max - min).y + 1;
}

void GameplayScene::draw() const {
  // TODO: show level background art?
  ClearBackground(DARKGREEN);

  draw_viewport();
  draw_hud();
}

UpdateResult GameplayScene::update(Input in) {
  const auto step = [&](Direction dir) -> UpdateResult {
    switch (state.step(dir)) {
    case StepResult::Ok:
      ++moves;
      break;

    case StepResult::LevelComplete:
      return UpdateTransition {
        .next = std::unique_ptr<Scene> {
          new LevelCompleteScene {level, moves}
        },
      };

    // TODO: play animations
    case StepResult::HitWall:
    case StepResult::InvalidDirection:
    case StepResult::PushTwoObjects:
    case StepResult::PushYourself:
    case StepResult::SlamOnDoor:
      break;
    }

    player_dir = dir;
    return UpdateOk {};
  };

  switch (in) {
  case Input::INPUT_UP:
    return step(Direction::Up);
  case Input::INPUT_DOWN:
    return step(Direction::Down);
  case Input::INPUT_LEFT:
    return step(Direction::Left);
  case Input::INPUT_RIGHT:
    return step(Direction::Right);

  case Input::INPUT_MENU:
  case Input::INPUT_BACK:
    return UpdateTransition {
      .next = std::unique_ptr<Scene> {new PauseScene {
        level,
        moves,
        *this,
      }}
    };
    break;

  // these don't do anything in gameplay
  case Input::INPUT_NONE:
  case Input::INPUT_ENTER:
    return UpdateOk {};
  }

  return UpdateOk {};
}

void GameplayScene::draw_viewport() const {
  const usize   screen_w = GetScreenWidth();
  const usize   screen_h = GetScreenHeight();
  const Vector2 screen_size {
    .x = static_cast<f32>(screen_w),
    .y = static_cast<f32>(screen_h),
  };

  // ===== draw viewport background =====

  const f32     level_aspect  = ([&]() {
    const f32 width_  = width;
    const f32 height_ = height;
    return width_ / height_;
  })();
  const Vector2 viewport_size = ([&]() -> Vector2 {
    const f32 screen_w_ = screen_w;
    const f32 screen_h_ = screen_h;
    if (screen_h_ * level_aspect > screen_w_)
      return {screen_w_, screen_w_ / level_aspect};
    else
      return {screen_h_ * level_aspect, screen_h_};
  })();
  const Vector2 viewport_pos  = (screen_size - viewport_size) / 2;
  DrawRectangleV(viewport_pos, viewport_size, BLACK);

  // ===== draw level grid ====

  const Vector2 level_size {
    .x = static_cast<f32>(width),
    .y = static_cast<f32>(height),
  };
  const Vector2 cell_size = viewport_size / level_size;
  const f32 sprite_scale  = cell_size.x / level.theme().tile_size();

  using Index = decltype(Position<>::x);
  for (Index y = 0; y < height; ++y)
    for (Index x = 0; x < width; ++x) {
      const Position<> cell_index_pos {
        .x = min.x + x,
        .y = min.y + y,
      };
      const Vector2 cell_pos {
        .x = viewport_pos.x + x * cell_size.x,
        .y = viewport_pos.y + y * cell_size.y,
      };

      const auto tile_sprite = tile_sprite_at(cell_index_pos);
      const auto obj_sprite_ = obj_sprite_at(cell_index_pos);

      DrawTextureEx(tile_sprite, cell_pos, 0, sprite_scale, WHITE);
      if (obj_sprite_) {
        const auto &obj_sprite = obj_sprite_.value();
        DrawTextureEx(obj_sprite, cell_pos, 0, sprite_scale, WHITE);
      }
    }
}

void GameplayScene::draw_hud() const {
  const Font font = GuiGetFont();

  const Vector2 ctr_view_pos {
    .x = 40,
    .y = 40,
  };
  const f32   ctr_font_size    = 24;
  const f32   ctr_spacing      = 6;
  const f32   ctr_line_spacing = 5;
  const f32   ctr_view_spacing = 15;
  const Color ctr_color        = WHITE;

  // ===== draw move counter =====

  const std::string_view move_ctr = "moves"sv;
  const std::string      move_cnt = std::format("{}", moves);

  const Vector2 move_ctr_size =
    MeasureTextEx(font, move_ctr.data(), ctr_font_size, ctr_spacing);
  const Vector2 move_cnt_size =
    MeasureTextEx(font, move_cnt.data(), ctr_font_size, ctr_spacing);
  const Vector2 move_ctr_view_size {
    .x = std::max(move_ctr_size.x, move_cnt_size.x),
    .y = std::max(move_ctr_size.y, move_cnt_size.y),
  };

  const Vector2 move_ctr_pos = ctr_view_pos;
  const Vector2 move_cnt_pos {
    .x = move_ctr_pos.x,
    .y = move_ctr_pos.y + move_ctr_size.y + ctr_line_spacing,
  };

  DrawTextEx(
    font,
    move_ctr.data(),
    move_ctr_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );
  DrawTextEx(
    font,
    move_cnt.data(),
    move_cnt_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );

  // ===== draw goal counter =====

  const std::string_view goal_ctr = "goals"sv;
  const std::string      goal_cnt = std::format("{}", 0);

  const Vector2 goal_ctr_size =
    MeasureTextEx(font, goal_ctr.data(), ctr_font_size, ctr_spacing);

  const Vector2 goal_ctr_pos {
    .x = ctr_view_pos.x + move_ctr_view_size.x + ctr_view_spacing,
    .y = ctr_view_pos.y,
  };
  const Vector2 goal_cnt_pos {
    .x = goal_ctr_pos.x,
    .y = goal_ctr_pos.y + goal_ctr_size.y + ctr_line_spacing,
  };

  DrawTextEx(
    font,
    goal_ctr.data(),
    goal_ctr_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );
  DrawTextEx(
    font,
    goal_cnt.data(),
    goal_cnt_pos,
    ctr_font_size,
    ctr_spacing,
    ctr_color
  );
}

template <typename T>
T select_dir(Direction dir, T up, T down, T left, T right) {
  switch (dir) {
  case Direction::Up:
    return up;
  case Direction::Down:
    return down;
  case Direction::Left:
    return left;
  case Direction::Right:
    return right;
  }
}

Texture GameplayScene::tile_sprite_at(Position<> pos) const {
  const auto &theme   = level.theme();
  const auto &sprites = theme.sprites();
  const auto  tile    = state.inner().find_tile(pos);

  if (!tile)
    return *sprites[theme.WALL];

  return std::visit(
    overload {
      [&](const Floor &) -> const auto & {
        return *sprites[theme.FLOOR];
      },
      [&](const Button &) -> const auto & {
        return *sprites[theme.BUTTON];
      },
      [&](const Door &door) -> const auto & {
        const bool open = state.inner().is_door_open(door.door_id);
        return *sprites[open ? theme.DOOR_OPEN : theme.DOOR_CLOSED];
      },
      [&](const Portal &portal) -> const auto & {
        return *sprites[select_dir(
          portal.in_dir,
          theme.PORTAL_N,
          theme.PORTAL_S,
          theme.PORTAL_W,
          theme.PORTAL_E
        )];
      },
      [&](const DirFloor &df) -> const auto & {
        return *sprites[select_dir(
          df.dir,
          theme.DIRFLOOR_N,
          theme.DIRFLOOR_S,
          theme.DIRFLOOR_W,
          theme.DIRFLOOR_E
        )];
      },
      [&](const Goal &) -> const auto & {
        return *sprites[theme.GOAL];
      }
    },
    tile.value()
  );
}

std::optional<Texture>
GameplayScene::obj_sprite_at(Position<> pos) const {
  const auto &theme   = level.theme();
  const auto &sprites = theme.sprites();
  const auto  obj     = state.inner().find_object(pos);

  if (!obj)
    return std::nullopt;

  return std::visit(
    overload {
      [&](const Player &) -> const auto & {
        return *sprites[select_dir(
          player_dir,
          theme.PLAYER_N,
          theme.PLAYER_S,
          theme.PLAYER_W,
          theme.PLAYER_E
        )];
      },
      [&](const Box &) -> const auto & {
        return *sprites[theme.BOX];
      },
      [&](const DirBox &db) -> const auto & {
        return *sprites[select_dir(
          db.dir,
          theme.DIRBOX_N,
          theme.DIRBOX_S,
          theme.DIRBOX_W,
          theme.DIRBOX_E
        )];
      },
    },
    obj.value()
  );
}

} // namespace sbokena::game::scene
