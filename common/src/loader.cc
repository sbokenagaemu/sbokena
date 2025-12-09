#include "loader.hh"

#include <filesystem>
#include <format>
#include <stdexcept>

#include <raylib.h>

namespace sbokena::loader {

// ===== load_res/unload_res impls =====

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

// ===== exceptions =====

ThemeLoadException::ThemeLoadException(
  std::string_view name, const fs::path &path
)
  : std::runtime_error {std::format(
      R"(failed to load theme "{}" from search root "{}")",
      name,
      path.c_str()
    )},
    name {name},
    path {path} {}

InvalidLevelException::InvalidLevelException()
  : std::runtime_error {"invalid level data"} {}

} // namespace sbokena::loader
