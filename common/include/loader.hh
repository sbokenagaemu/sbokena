// loaders & validators.

#pragma once

#include <array>
#include <filesystem>
#include <map>
#include <optional>
#include <string_view>

#include <raylib.h>

#include "level.hh"
#include "position.hh"

namespace fs = std::filesystem;

using namespace sbokena::level;
using sbokena::position::Position;

namespace sbokena::loader {

// a loaded and validated theme for in-memory use.
// a value of `loader::Theme` guarantees that:
// - all `Tile`/`Object` textures are squares.
// - all `Tile`/`Object` textures have the same dimensions.
// - all textures listed in `themes/LAYOUT.md` are present.
class Theme {
public:
  Theme()                         = delete;
  Theme(const Theme &)            = default;
  Theme &operator=(const Theme &) = default;
  Theme(Theme &&)                 = default;
  Theme &operator=(Theme &&)      = default;

  ~Theme();

  // load a theme with a given name from a `themes` directory, without
  // any additional probing of parent directories.
  static std::optional<Theme> load_norec(
    const std::string_view name, const fs::path &themes_dir
  ) noexcept;

  // load a theme with a given name
  static std::optional<Theme> load(
    const std::string_view name, const fs::path &search_root
  ) noexcept;

  // ===== textures array indices =====
  //
  // WARN: update the array in `Theme::load_norec` if you're changing
  // the order of these, or add a new one.

  static constexpr usize FLOOR       = 0;
  static constexpr usize BUTTON      = 1;
  static constexpr usize DOOR_OPEN   = 2;
  static constexpr usize DOOR_CLOSED = 3;
  static constexpr usize PORTAL_N    = 4;
  static constexpr usize PORTAL_E    = 5;
  static constexpr usize PORTAL_W    = 6;
  static constexpr usize PORTAL_S    = 7;
  static constexpr usize DIRFLOOR_N  = 8;
  static constexpr usize DIRFLOOR_E  = 9;
  static constexpr usize DIRFLOOR_W  = 10;
  static constexpr usize DIRFLOOW_S  = 11;
  static constexpr usize GOAL        = 12;
  static constexpr usize WALL        = 13;
  static constexpr usize ROOF        = 14;
  static constexpr usize PLAYER_N    = 15;
  static constexpr usize PLAYER_E    = 16;
  static constexpr usize PLAYER_W    = 17;
  static constexpr usize PLAYER_S    = 18;
  static constexpr usize BOX         = 19;
  static constexpr usize DIRBOX_N    = 20;
  static constexpr usize DIRBOX_E    = 21;
  static constexpr usize DIRBOX_W    = 22;
  static constexpr usize DIRBOX_S    = 23;

  // the textures associated with the theme.
  //
  // the returned span should be indexed with the constants defined on
  // the class.
  std::span<const Texture> textures() const noexcept {
    return textures_;
  }

  // the name of the theme.
  std::string_view name() const noexcept {
    return name_;
  }

private:
  std::string             name_;
  std::array<Texture, 24> textures_;

  Theme(const std::string_view &name) : name_ {name}, textures_ {} {}
};

}; // namespace sbokena::loader
