// loaders & validators.

#pragma once

#include <array>
#include <concepts>
#include <format>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <raylib.h>

#include "position.hh"
#include "types.hh"

namespace fs = std::filesystem;

using namespace sbokena::types;
using sbokena::position::Position;

namespace sbokena::loader {

template <typename Res>
Res load_res(const fs::path &path);
template <typename Res>
void unload_res(Res &&res);

template <>
Image load_res<Image>(const fs::path &path) {
  return LoadImage(path.c_str());
}

template <>
Texture load_res<Texture>(const fs::path &path) {
  return LoadTexture(path.c_str());
}

template <>
void unload_res<Image>(Image &&img) {
  UnloadImage(img);
}

template <>
void unload_res<Texture>(Texture &&tex) {
  UnloadTexture(tex);
}

// clang-format off

// a loadable and once-unloadable resource.
template <typename Res>
concept Resource = requires(const fs::path &path, Res &&res) {
  { load_res<Res>(path) } -> std::same_as<Res>;
  { unload_res(res)     } -> std::same_as<void>;
};

// a sprite with a width and height.
template <typename S>
concept Sprite = requires(const S &sprite) {
  { sprite.width  } -> std::convertible_to<usize>;
  { sprite.height } -> std::convertible_to<usize>;
};

// clang-format on

// an RAII-style owned resource.
template <Resource Res>
class OwnedResource {
public:
  // load a fresh resource from a file path.
  OwnedResource(const fs::path &path)
    : res {std::move(load_res<Res>(path))} {}

  // take over an unmanaged resource.
  OwnedResource(Res &&res) : res {std::move(res)} {}

  // create an empty (invalid) owned resource.
  // destroying this is fine.
  OwnedResource()                                 = default;
  OwnedResource(const OwnedResource &)            = delete;
  OwnedResource &operator=(const OwnedResource &) = delete;

  OwnedResource(OwnedResource &&other) : res {std::move(other.res)} {
    other.res.reset();
  }

  OwnedResource &operator=(OwnedResource &&other) {
    res = std::move(other.res);
    other.res.reset();
  }

  // unloads the managed resource.
  ~OwnedResource() {
    if (res)
      unload_res(res);
  }

  std::optional<Res> &get() noexcept {
    return res.and_then([](Res &res) { return res; });
  }

  std::optional<const Res> &get_const() const noexcept {
    return res.and_then([](const Res &res) { return res; });
  }

  Res &operator*() {
    return res.value();
  }

  const Res &operator*() const {
    return res.value();
  }

  std::optional<Res> &&take() noexcept {
    return res.and_then(std::move);
  }

private:
  std::optional<Res> res;
};

// an exception thrown when constructing a `Theme`.
struct ThemeLoadException : std::runtime_error {
  ThemeLoadException(std::string_view name, const fs::path &path)
    : std::runtime_error {std::format(
        "failed to load theme \"{}\" from search root \"{}\"",
        name,
        path.c_str()
      )},
      name {name},
      path {path} {}

  const std::string name;
  const fs::path    path;
};

// a loaded and validated theme for in-memory use.
// a value of `loader::Theme` guarantees that:
// - all `Tile`/`Object` sprites are squares.
// - all `Tile`/`Object` sprites have the same dimensions.
// - all textures listed in `themes/LAYOUT.md` are present.
template <typename S>
  requires Resource<S> && Sprite<S>
class Theme {
public:
  // clang-format off

  // indices into the sprites array.
  enum SpriteIndex {
    FLOOR,      BUTTON,     DOOR_OPEN,  DOOR_CLOSED,
    PORTAL_N,   PORTAL_E,   PORTAL_W,   PORTAL_S,
    DIRFLOOR_N, DIRFLOOR_E, DIRFLOOR_W, DIRFLOOR_S,
    GOAL,       WALL,       ROOF,

    PLAYER_N,   PLAYER_E,   PLAYER_W,   PLAYER_S,
    BOX,
    DIRBOX_N,   DIRBOX_E,   DIRBOX_W,   DIRBOX_S,

    // number of sprites in a theme.
    // not really intended for public use.
    __SPRITES,
  };

  // basenames for sprite files.
  static constexpr std::array<std::string_view, __SPRITES> names = {
    "floor",      "button",     "door_open",  "door_closed",
    "portal_n",   "portal_e",   "portal_w",   "portal_s",
    "dirfloor_n", "dirfloor_e", "dirfloor_w", "dirfloor_s",
    "goal",       "wall",       "roof",

    "player_n",   "player_e",   "player_w",   "player_s",
    "box",
    "dirbox_n",   "dirbox_e",   "dirbox_w",   "dirbox_s",
  };

  // clang-format on

  // type of the backing sprite storage.
  using Sprites = std::array<OwnedResource<S>, __SPRITES>;

  // load a named theme from a path, seeking upwards in the directory
  // tree recursively until it's found.
  Theme(std::string_view name, const fs::path &search_root) {
    fs::path cur {search_root};
  }

  // conversion between resource types.
  // mostly useful for committing sprites to VRAM.
  template <typename T>
    requires Resource<T> && Sprite<T>
  Theme(const Theme<T> &);

  // commit a Theme<Image> to a Theme<Texture>, i.e.
  // store all sprites in VRAM.
  template <>
  Theme<Texture>(const Theme<Image> &theme)
    : name_ {name_},
      sprites_ {std::make_shared_for_overwrite(__SPRITES)} {
    for (usize i = 0; i < __SPRITES; ++i)
      sprites_[i] = LoadTextureFromImage(*theme.sprites()[i]);
  }

  Theme()                         = delete;
  Theme(const Theme &)            = default;
  Theme &operator=(const Theme &) = default;
  Theme(Theme &&)                 = default;
  Theme &operator=(Theme &&)      = default;
  ~Theme()                        = default;

  // the name of the theme.
  std::string_view name() const noexcept {
    return *name_;
  }

  // the sprites contained in the theme.
  const Sprites &sprites() const noexcept {
    return *sprites_;
  }

private:
  std::shared_ptr<std::string> name_;
  std::shared_ptr<Sprites>     sprites_;
};

}; // namespace sbokena::loader
