#include <filesystem>

#include <raylib.h>

#include "Texture.hpp"
#include "loader.hh"

namespace fs = std::filesystem;

static bool is_valid_folder(fs::path dir) {
  return (!(!fs::exists(dir) || !fs::is_directory(dir)));
}

// this function checks whether the given folder is an acceptable theme pack
bool is_valid_theme(fs::path dir) {
  // checks for the 3 main folders: directionals, other and portals
  if ((!is_valid_folder(dir / "directionals"))) {
    return false;
  }

  // clang-format off
  // checks for the 2 subdirectories: dir_floors and dir_boxes
  fs::path dir_directional = dir / "directionals";
  if ( !is_valid_folder(dir_directional / "dir_floors") 
    || !is_valid_folder(dir_directional / "dir_boxes"))
    return false;

  // checks for 4 directional box textures
  fs::path dir_box = dir_directional / "dir_boxes";
  if ( !fs::exists(dir_box / "dir_box_up.png")
    || !fs::exists(dir_box / "dir_box_right.png")
    || !fs::exists(dir_box / "dir_box_down.png")
    || !fs::exists(dir_box / "dir_box_left.png"))
    return false;

  // checks for 4 directional floor textures
  fs::path dir_floor = dir_directional / "dir_floors";
  if ( !fs::exists(dir_floor / "dir_floor_up.png")
    || !fs::exists(dir_floor / "dir_floor_right.png")
    || !fs::exists(dir_floor / "dir_floor_down.png")
    || !fs::exists(dir_floor / "dir_floor_left.png"))
    return false;

  // checks for other necessary textures
  if (!fs::exists(dir / "button.png")
    || !fs::exists(dir / "box.png")
    || !fs::exists(dir / "door_closed.png")
    || !fs::exists(dir / "door_open.png")
    || !fs::exists(dir / "player.png")
    || !fs::exists(dir / "roof.png")
    || !fs::exists(dir / "tile.png")
    || !fs::exists(dir / "wall.png")
    || !fs::exists(dir / "portal.png"))
    return false;
  // clang-format on

  return true;
}

std::vector<std::string> valid_textures() {
  fs::path texture_dir = std::filesystem::current_path() / "common/res";
  std::vector<std::string> names = {};

  if (!is_valid_folder(texture_dir)) {
    return names;
  }

  for (const auto &entry : fs::directory_iterator(texture_dir))
    if (is_valid_theme(entry))
      names.push_back(entry.path().filename());

  return names;
}

Theme::Theme(std::string name) : name_{name} {
  fs::path dir = std::filesystem::current_path() / "common/res" / name;

  textures_[0] = raylib::Texture(dir / "roof.png");
  textures_[1] = raylib::Texture(dir / "wall.png");
  textures_[2] = raylib::Texture(dir / "tile.png");
  textures_[3] = raylib::Texture(dir / "player.png");
  textures_[4] = raylib::Texture(dir / "box.png");
  textures_[5] = raylib::Texture(dir / "button.png");
  textures_[6] = raylib::Texture(dir / "door_open.png");
  textures_[7] = raylib::Texture(dir / "door_closed.png");

  fs::path dir_floor = dir / "directionals" / "dir_floors";
  textures_[8] = raylib::Texture(dir_floor / "dir_floor_up.png");
  textures_[9] = raylib::Texture(dir_floor / "dir_floor_right.png");
  textures_[10] = raylib::Texture(dir_floor / "dir_floor_down.png");
  textures_[11] = raylib::Texture(dir_floor / "dir_floor_left.png");

  fs::path dir_box = dir / "directionals" / "dir_boxes";
  textures_[12] = raylib::Texture(dir_box / "dir_box_up.png");
  textures_[13] = raylib::Texture(dir_box / "dir_box_right.png");
  textures_[14] = raylib::Texture(dir_box / "dir_box_down.png");
  textures_[15] = raylib::Texture(dir_box / "dir_box_left.png");

  textures_[16] = raylib::Texture(dir / "portal.png");
};