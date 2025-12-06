// loaders & validators.

#pragma once

#include <array>
#include <concepts>
#include <filesystem>
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <print>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <nlohmann/json.hpp>
#include <raylib.h>

#include "level.hh"
#include "position.hh"
#include "types.hh"
#include "utils.hh"

namespace fs = std::filesystem;

using nlohmann::json;

using namespace sbokena::types;
using namespace sbokena::utils;
using namespace sbokena::position;
namespace level = sbokena::level;

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

// an exception thrown when validating a `Level`.
struct InvalidLevelException : std::runtime_error {
  InvalidLevelException()
    : std::runtime_error {"invalid level data"} {}
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

// a loaded and validated level for in-memory use.
// a value of `loader::Level` guarantees that:
// - the requisite theme is valid and loaded. see the `Theme` class
//   for the exact details of these guarantees.
// - all `Door` tiles have at least one corresponding button.
// - all `Portal` tiles are paired correctly.
// - the number of `Goal` tiles equals the number of `Box` and
//  `DirBox` objects combined.
// - there is exactly one `Player` object.
template <typename S>
  requires Resource<S> && Sprite<S>
class Level {
public:
  using DoorSet = std::pair<
    Position<>,          // position of door
    std::set<Position<>> // positions of buttons
    >;
  using PortalSet = std::pair<
    Position<>, // position of 1st portal
    Position<>  // position of 2nd portal
    >;

  // validate a `RawLevel`, and take in a `Theme`.
  //
  // this constructor is parameterized, i.e. you can pass in a
  // `Theme<Image>` to a `Level<Texture>` constructor, and the theme
  // will be committed into VRAM.
  template <typename T = S>
    requires Resource<T> && Sprite<T>
  Level(const level::Level &raw, const Theme<T> &theme)
    : name_ {raw.name},
      theme_ {theme},
      diff_ {raw.diff},
      tiles_ {raw.tiles},
      objects_ {raw.objects},
      player_ {POS_MAX<>} {
    // #box must equal #goal
    isize boxes = 0;

    for (const auto &[pos, obj] : objects_)
      switch (obj.index()) {
      case index_of<level::Object, level::Player>(): {
        // check if we've already set the player's position
        assert_throw(player_ == POS_MAX<>, InvalidLevelException {});
        player_ = pos;
        break;
      }
      case index_of<level::Object, level::Box>():
      case index_of<level::Object, level::DirBox>(): {
        // count #box
        ++boxes;
        break;
      }
      }

    for (const auto &[pos, tile] : tiles_)
      switch (tile.index()) {
      case index_of<level::Tile, level::Floor>():
        // nothing to do here
        break;
      case index_of<level::Tile, level::Button>(): {
        // - if the door is there, add the button
        // - if the door is not there, insert a fake door
        const auto &btn  = std::get<level::Button>(tile);
        const auto  iter = doors_.find(btn.door_id);
        if (iter != doors_.end())
          iter->second.second.insert(pos);
        else
          doors_.insert({btn.door_id, {POS_MAX<>, {pos}}});
        break;
      }
      case index_of<level::Tile, level::Door>(): {
        // - if the door is not there, add an empty set
        // - if the door is there, either it's a placeholder (ok), or
        //   it's a duplicate (error)
        const auto &door = std::get<level::Door>(tile);
        const auto  iter = doors_.find(door.door_id);
        if (iter != doors_.end()) {
          assert_throw(
            iter->second.first == POS_MAX<>, InvalidLevelException {}
          );
          iter->second.first = pos;
          break;
        }
        break;
      }
      case index_of<level::Tile, level::Portal>(): {
        // - if the pair isn't present, add as the left hand
        // - if the right hand is filled, add as the right hand
        // - if both are filled, throw an error.
        const auto &portal = std::get<level::Portal>(tile);
        const auto  iter   = portals_.find(portal.portal_id);
        if (iter == portals_.end())
          portals_.insert({portal.portal_id, {pos, POS_MAX<>}});
        else {
          assert_throw(
            iter->second.second == POS_MAX<>, InvalidLevelException {}
          );
          iter->second.second = pos;
        }
        break;
      };
      case index_of<level::Tile, level::Goal>(): {
        // "uncount" #box
        // if it's 0 at the end, the level is valid
        --boxes;
        break;
      }
      }

    // require #box == #goal
    assert_throw(boxes == 0, InvalidLevelException {});

    // require player to be present
    assert_throw(player_ != POS_MAX<>, InvalidLevelException {});

    // check for dangling doorsets
    for (const auto &[_, doorset] : doors_) {
      assert_throw(
        doorset.first != POS_MAX<>, InvalidLevelException {}
      );
      assert_throw(!doorset.second.empty(), InvalidLevelException {});
    }

    // check for dangling portals
    for (const auto &[_, portalset] : portals_) {
      assert_throw(
        portalset.first != POS_MAX<>, InvalidLevelException {}
      );
      assert_throw(
        portalset.second != POS_MAX<>, InvalidLevelException {}
      );
    }
  }

  Level()                         = delete;
  Level(const Level &)            = default;
  Level &operator=(const Level &) = default;
  Level(Level &&)                 = default;
  Level &operator=(Level &&)      = default;
  ~Level()                        = default;

  std::string_view name() const noexcept {
    return name_;
  }

private:
  std::string       name_;
  Theme<S>          theme_;
  level::Difficulty diff_;

  std::map<Position<>, level::Tile>   tiles_;
  std::map<Position<>, level::Object> objects_;

  std::unordered_map<u32, DoorSet>   doors_;
  std::unordered_map<u32, PortalSet> portals_;

  Position<> player_;
};

}; // namespace sbokena::loader
