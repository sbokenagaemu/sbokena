#include <filesystem>
#include <format>
#include <iostream>

#include "loader.hh"
#include "raylib.h"

namespace fs = std::filesystem;

bool isValidFolder(fs::path dir) {
  return (!fs::exists(dir) || !fs::is_directory(dir));
}

std::vector<Image> TextureID::GetTextures(int id) {
  return themes_[id];
}

TextureID::TextureID() {
  load_textures();
}

void TextureID::load_textures() {
  fs::path textureDir = "common/res";

  if (!isValidFolder(textureDir)) {
    return;
  }

  int theme_count = 0;
  for (const auto &entry : fs::directory_iterator(textureDir)) {
    theme_count++;
  }
  TextureID::theme_count_ = theme_count;
}
