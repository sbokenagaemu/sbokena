#include <filesystem>
#include <format>
#include <iostream>

#include "loader.hh"

namespace fs = std::filesystem;

int load_textures(std::string_view path) {
  fs::path textureDir = path;
  std::cout << "Absolute: " << fs::absolute(textureDir) << "\n";

  if (!fs::exists(textureDir)) {
    throw std::format_error("Error: folder not found.");
    return 0;
  }

  int theme_count = 0;
  for (const auto &entry : fs::directory_iterator(textureDir)) {
    theme_count++;
  }
  return theme_count;
}
