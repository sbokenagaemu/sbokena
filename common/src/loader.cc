#include <filesystem>
#include <format>
#include <iostream>

#include "loader.hh"
#include "raylib.h"

namespace fs = std::filesystem;

bool isValidFolder(fs::path dir) {
  return (!fs::exists(dir) || !fs::is_directory(dir));
}

// this function checks whether the given folder is an acceptable theme pack
bool isValidTheme(fs::path dir) {
  // checks for the 3 main folders: directionals, other and portals
  if ((!isValidFolder(dir / "directionals")) ||
      (!isValidFolder(dir / "other")) || (!isValidFolder(dir / "portals"))) {
    return false;
  }

  // checks for the 2 subdirectories: dir_floors and dir_crates
  fs::path dirDirectional = dir / "directionals";
  if ((!isValidFolder(dirDirectional / "dir_floors")) ||
      (!isValidFolder(dirDirectional / "dir_crates"))) {
    return false;
  }

  // checks for 4 directional crate textures
  fs::path dirCrate = dirDirectional / "dir_crates";
  if (!fs::exists(dirCrate / "dir_create_up.png") ||
      !fs::exists(dirCrate / "dir_create_right.png") ||
      !fs::exists(dirCrate / "dir_create_down.png") ||
      !fs::exists(dirCrate / "dir_create_left.png")) {
    return false;
  }

  // checks for 4 directional floor textures
  fs::path dirFloors = dirDirectional / "dir_floors";
  if (!fs::exists(dirFloors / "dir_floor_up.png") ||
      !fs::exists(dirFloors / "dir_floor_right.png") ||
      !fs::exists(dirFloors / "dir_floor_down.png") ||
      !fs::exists(dirFloors / "dir_floor_left.png")) {
    return false;
  }

  // checks for 3 portal textures
  fs::path dirPortals = dir / "portals";
  if (!fs::exists(dirPortals / "portal_1.png") ||
      !fs::exists(dirPortals / "portal_2.png") ||
      !fs::exists(dirPortals / "portal_3.png")) {
    return false;
  }

  // checks for other necessary textures
  fs::path dirOthers = dirDirectional / "other";
  if (!fs::exists(dirOthers / "crate.png") ||
      !fs::exists(dirOthers / "door_closed.png") ||
      !fs::exists(dirOthers / "door_open.png") ||
      !fs::exists(dirOthers / "player.png") ||
      !fs::exists(dirOthers / "tile.png") ||
      !fs::exists(dirOthers / "wall.png") ||
      !fs::exists(dirOthers / "button.png") ||
      !fs::exists(dirOthers / "roof.png")) {
    return false;
  }

  return true;
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
