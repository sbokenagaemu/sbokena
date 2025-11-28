#include "loader.hh"

#include <algorithm>
#include <filesystem>
#include <optional>
#include <print>
#include <string_view>

#include <raylib.h>

namespace fs = std::filesystem;

namespace sbokena::loader {

Theme::~Theme() {
  for (auto &t : textures_)
    if (IsTextureValid(t))
      UnloadTexture(t);
}

std::optional<Theme> Theme::load_norec(
  const std::string_view name, const fs::path &themes_dir
) noexcept {
  try {
    Theme theme {name};

    // track texture size uniformity requirement
    std::optional<usize> size {std::nullopt};
    usize                loaded = 0;

    for (const auto &entry :
         fs::directory_iterator(themes_dir / name)) {
      // texture names, following the order of the constants.
      static constexpr std::array<std::string_view, 24> names = {
        "floor",      "button",     "door_open",  "door_closed",
        "portal_n",   "portal_e",   "portal_w",   "portal_s",
        "dirfloor_n", "dirfloor_e", "dirfloor_w", "dirfloor_s",
        "goal",       "wall",       "roof",       "player_n",
        "player_e",   "player_w",   "player_s",   "box",
        "dirbox_n",   "dirbox_e",   "dirbox_w",   "dirbox_s",
      };

      const fs::path path = entry.path();
      const fs::path stem = path.filename().stem();
      const auto &iter = std::find(names.begin(), names.end(), stem);
      if (iter == names.end()) {
        std::println("skipping {}", path.c_str());
        continue;
      }

      const usize   index = std::distance(names.begin(), iter);
      const Texture tex   = LoadTexture(path.c_str());

      // all textures must be square
      if (tex.width != tex.height) {
        UnloadTexture(tex);
        return std::nullopt;
      }

      // all textures must have the same size
      if (!size)
        size = tex.width;
      else if (tex.width != size) {
        UnloadTexture(tex);
        return std::nullopt;
      }

      ++loaded;
      theme.textures_[index] = tex;
    }

    // all textures must be present
    if (loaded < theme.textures_.size())
      return std::nullopt;

    return theme;
  } catch (std::exception &ex) {
    std::println("Theme::load_norec error: {}", ex.what());
    return std::nullopt;
  }
}

std::optional<Theme> Theme::load(
  const std::string_view name, const fs::path &search_root
) noexcept {
  try {
    std::optional<Theme> theme {std::nullopt};
    fs::path             search_path {search_root};

    while (!theme) {
      std::optional<Theme> found =
        load_norec(name, search_path / "themes");
      if (found) {
        theme = found;
        break;
      }

      if (!search_path.has_parent_path())
        break;

      search_path = search_path.parent_path();
    }

    return theme;
  } catch (std::exception &ex) {
    std::println("Theme::load error: {}", ex.what());
    return std::nullopt;
  }
}

} // namespace sbokena::loader
