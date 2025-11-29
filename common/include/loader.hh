// loaders & validators.

#pragma once

#include <array>
#include <format>
#include <memory>
#include <print>
#include <string>

#include <raylib.h>

#include "position.hh"

namespace fs = std::filesystem;

using sbokena::position::Position;

namespace sbokena::loader {

// ===== Resource & ManagedResource =====

template <typename Res>
Res load_res(const fs::path &);
template <typename Res>
void unload_res(Res &);

// a loadable and unloadable resource.
// `unload_res` must not be called twice on the same resource.
template <typename Res>
concept Resource = requires(const fs::path &path, Res &res) {
                     { load_res<Res>(path) } -> std::same_as<Res>;
                     { unload_res(res) } -> std::same_as<void>;
                   };

// a type which has a `width` and a `height` field.
template <typename T>
concept HasSize = requires(const T &val) {
                    { val.width } -> std::convertible_to<usize>;
                    { val.height } -> std::convertible_to<usize>;
                  };

// an automatically managed, shared `Resource`.
template <Resource Res>
class ManagedResource {
public:
  // construct a managed resource from an existing unmanaged one.
  ManagedResource(Res &&res)
    : res {make_shared_res(std::move(res))} {}

  // load a managed resource from a path.
  ManagedResource(const fs::path &path)
    : res {make_shared_res(std::move(load_res<Res>(path)))} {}

  ManagedResource()                                   = delete;
  ManagedResource(const ManagedResource &)            = default;
  ManagedResource &operator=(const ManagedResource &) = default;
  ManagedResource(ManagedResource &&)                 = default;
  ManagedResource &operator=(ManagedResource &&)      = default;
  ~ManagedResource()                                  = default;

  // get a const ref to the unmanaged resource.
  //
  // this class frees the resource on destruction, so the caller must
  // not use this reference past the lifetime of this object.
  const Res &inner() const noexcept {
    return *res;
  }

private:
  std::shared_ptr<Res> res;

  // `std::make_shared`, but also unloads `Res` on final deletion.
  std::shared_ptr<Res> make_shared_res(Res &&res) {
    return std::shared_ptr<Res> {std::move(res), unload_res<Res>};
  }
};

template <>
Image load_res<Image>(const fs::path &path) {
  return LoadImage(path.c_str());
}

template <>
Texture load_res<Texture>(const fs::path &path) {
  return LoadTexture(path.c_str());
}

template <>
void unload_res<Image>(Image &img) {
  UnloadImage(img);
}

template <>
void unload_res<Texture>(Texture &tex) {
  UnloadTexture(tex);
}

// ===== Theme =====

struct ThemeLoadException : std::runtime_error {
  ThemeLoadException(std::string_view name, const fs::path &path)
    : std::runtime_error {std::format(
        "loading theme {} from path {} failed", name, path.c_str()
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
template <typename Sprite>
  requires Resource<Sprite> && HasSize<Sprite>
class Theme {
public:
  // indices into the sprites array.
  //
  // NOTE: this must be kept in sync and in order with `names`.
  enum SpriteIndex : usize {
    FLOOR = 0,
    BUTTON,
    DOOR_OPEN,
    DOOR_CLOSED,
    PORTAL_N,
    PORTAL_E,
    PORTAL_W,
    PORTAL_S,
    DIRFLOOR_N,
    DIRFLOOR_E,
    DIRFLOOR_W,
    DIRFLOOR_S,
    GOAL,
    WALL,
    ROOF,
    PLAYER_N,
    PLAYER_E,
    PLAYER_W,
    PLAYER_S,
    BOX,
    DIRBOX_N,
    DIRBOX_E,
    DIRBOX_W,
    DIRBOX_S,
    // number of sprites
    __N_SPRITES,
  };

  // file stems for all sprites.
  //
  // NOTE: this must be kept in sync and in order with `SpriteIndex`.
  static constexpr std::array<std::string_view, __N_SPRITES> names = {
    "floor",      "button",     "door_open",  "door_closed",
    "portal_n",   "portal_e",   "portal_w",   "portal_s",
    "dirfloor_n", "dirfloor_e", "dirfloor_w", "dirfloor_s",
    "goal",       "wall",       "roof",       "player_n",
    "player_e",   "player_w",   "player_s",   "box",
    "dirbox_n",   "dirbox_e",   "dirbox_w",   "dirbox_s",
  };

  using ManagedSprite = ManagedResource<Sprite>;
  using Sprites       = std::array<ManagedSprite, __N_SPRITES>;

  // find and load a theme, given a name and a search root directory.
  //
  // this method looks for `themes/<name>` in the given directory and
  // all of its parents, stopping at the first one it finds and loads
  // successfully.
  //
  // if the theme cannot be found/loaded, this method throws a
  // `ThemeLoadException`.
  Theme(std::string_view name, const fs::path &search_root)
    : name_ {name} {
    std::optional<Sprites> sprites {};
    fs::path               current_root {search_root};

    // make sure current_root is a directory and not a file.
    if (current_root.has_filename())
      current_root = current_root.parent_path();

    while (!sprites) {
      // load the theme from `current_root/themes/name`

      Sprites              tmp;
      usize                loaded = 0;
      std::optional<usize> width;
      const fs::path       themes = current_root / "themes";
      const fs::path       dir    = themes / name;

      if (!fs::exists(dir)) {
        std::println("theme dir not found in {}", themes.c_str());
        goto next;
      }

      for (const auto &entry : fs::directory_iterator(dir)) {
        const fs::path path = entry.path();
        const fs::path stem = path.stem();

        const auto &iter =
          std::find(names.begin(), names.end(), stem);
        if (iter == names.end()) {
          std::println("skipping irrelevant file {}", path.c_str());
          continue;
        }

        const ManagedSprite sprite {path};
        const Sprite        sprite_ = sprite.inner();

        if (sprite_.width != sprite_.height) {
          std::println("non-square sprite {}", path.c_str());
          goto next;
        }

        if (!width)
          width = sprite_.width;
        else if (width.value() != sprite_.width) {
          std::println("non-uniform sprite {}", dir.c_str());
          goto next;
        }

        const usize index = std::distance(names.begin(), iter);
        tmp[index]        = std::move(sprite);
        ++loaded;
      }

      if (loaded < __N_SPRITES) {
        std::println("theme {} missing sprites", dir.c_str());
        goto next;
      }

      sprites = std::move(tmp);
      continue;

    next:
      if (current_root.has_parent_path())
        current_root = current_root.parent_path();
      else
        break;
    }

    if (!sprites)
      throw ThemeLoadException {name, search_root};
    else
      sprites_ = sprites.value();
  }

  Theme()                         = delete;
  Theme(const Theme &)            = default;
  Theme &operator=(const Theme &) = default;
  Theme(Theme &&)                 = default;
  Theme &operator=(Theme &&)      = default;
  ~Theme()                        = default;

  // the name of the theme.
  std::string_view name() const noexcept {
    return name_;
  }

  // the sprites contained within the theme.
  std::span<const ManagedSprite, __N_SPRITES>
  sprites() const noexcept {
    return sprites_;
  }

private:
  std::string   name_;
  const Sprites sprites_;
};

}; // namespace sbokena::loader
