// loaders & validators.

#pragma once

#include <array>
#include <concepts>
#include <format>
#include <memory>
#include <optional>
#include <print>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <raylib.h>

#include "position.hh"
#include "types.hh"
#include "utils.hh"

namespace fs = std::filesystem;

using namespace sbokena::types;
using namespace sbokena::utils;
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
    return *this;
  }

  // unloads the managed resource.
  ~OwnedResource() {
    if (res)
      unload_res(std::move(res.value()));
  }

  // take the resource back.
  Res &&take() {
    Res &&moved = std::move(res.value());
    res.reset();
    return moved;
  }

  // access the underlying resource.
  Res &operator*() {
    return res.value();
  }

  // access the underlying resource.
  const Res &operator*() const {
    return res.value();
  }

  // access the underlying resource.
  Res *operator->() noexcept {
    return res ? &res.value() : nullptr;
  }

  // access the underlying resource.
  const Res *operator->() const noexcept {
    return res ? &res.value() : nullptr;
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
// a valid value of `loader::Theme` guarantees that:
// - all `Tile`/`Object` sprites have the same non-zero, uniform,
//   square sizes.
// - all sprites listed in `themes/LAYOUT.md` are present.
//
// note that the default-constructed `Theme` is NOT valid.
// trying to call any of its accessor methods will throw an exception.
template <typename S>
  requires Resource<S> && Sprite<S>
class Theme {
public:
  // the owned sprite type.
  using OwnedSprite = OwnedResource<S>;
  // the backing sprites store.
  using Sprites = OwnedSprite[];

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

  // load a named theme from a path, seeking upwards in the directory
  // tree recursively until it's found.
  Theme(std::string_view name, const fs::path &search_root)
    : name_ {std::make_shared<std::string>(name)},
      sprites_ {nullptr} {
    fs::path                 cur = fs::canonical(search_root);
    std::unique_ptr<Sprites> tmp {nullptr};

    while (!tmp) {
      tmp          = std::make_unique<Sprites>(__SPRITES);
      usize loaded = 0;
      usize width  = 0;

      const fs::path dir = cur / "themes" / name;
      if (!fs::exists(dir))
        goto backtrack;

      for (const auto &entry : fs::directory_iterator {dir}) {
        const fs::path path = entry.path();
        const fs::path stem = path.stem();

        // skip irrelevant files
        const auto iter = std::find(names.begin(), names.end(), stem);
        if (iter == names.end()) {
          std::println("ignoring file {}", path.c_str());
          continue;
        }

        const usize i = std::distance(names.begin(), iter);
        tmp[i]        = path;

        // all sprites must have positive size
        if (!tmp[i]->width || !tmp[i]->height) {
          std::println("file {} has zero size", path.c_str());
          goto backtrack;
        }

        // all sprites must be square
        if (tmp[i]->width != tmp[i]->height) {
          std::println("file {} has non-square size", path.c_str());
          goto backtrack;
        }

        // all sprites must have the same size
        if (!width)
          width = tmp[i]->width;
        else if (static_cast<usize>(tmp[i]->width) != width) {
          std::println("file {} has non-uniform size", path.c_str());
          goto backtrack;
        }

        ++loaded;
      }

      if (loaded < __SPRITES)
        goto backtrack;

      break;

    backtrack:
      std::println("{} invalid, backtracking...", cur.c_str());
      tmp.reset();
      if (cur == cur.root_path())
        break;
      cur = cur.parent_path();
    }

    assert_throw(!!tmp, ThemeLoadException {name, search_root});
    sprites_ = std::move(tmp);
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
    : name_ {theme.name_},
      sprites_ {std::make_shared_for_overwrite<Sprites>(__SPRITES)} {
    for (usize i = 0; i < __SPRITES; ++i)
      sprites_[i] = LoadTextureFromImage(*theme.sprites_[i]);
  }

  // the sole invalid `Theme`.
  Theme() : name_ {nullptr}, sprites_ {nullptr} {}

  Theme(const Theme &)            = default;
  Theme &operator=(const Theme &) = default;
  Theme(Theme &&)                 = default;
  Theme &operator=(Theme &&)      = default;
  ~Theme()                        = default;

  // the name of the theme.
  std::string_view name() const {
    assert_throw(!!name_);
    return *name_;
  }

  // the sprites contained in the theme.
  std::span<OwnedSprite, __SPRITES> sprites() const {
    assert_throw(!!sprites_);
    return std::span<OwnedSprite, __SPRITES> {
      sprites_.get(), __SPRITES
    };
  }

private:
  std::shared_ptr<std::string> name_;
  std::shared_ptr<Sprites>     sprites_;

  template <typename T>
    requires Resource<T> && Sprite<T>
  friend class Theme;
};

}; // namespace sbokena::loader
