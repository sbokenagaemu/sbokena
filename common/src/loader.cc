#include <filesystem>

#include <raylib.h>

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
  fs::path texture_dir = fs::current_path() / "common/res";
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
  fs::path dir = fs::current_path() / "common/res" / name;

  textures_[0] = LoadTexture((dir / "roof.png").c_str());
  textures_[1] = LoadTexture((dir / "wall.png").c_str());
  textures_[2] = LoadTexture((dir / "tile.png").c_str());
  textures_[3] = LoadTexture((dir / "player.png").c_str());
  textures_[4] = LoadTexture((dir / "box.png").c_str());
  textures_[5] = LoadTexture((dir / "button.png").c_str());
  textures_[6] = LoadTexture((dir / "door_open.png").c_str());
  textures_[7] = LoadTexture((dir / "door_closed.png").c_str());

  fs::path dir_floor = dir / "directionals" / "dir_floors";
  textures_[8] = LoadTexture((dir_floor / "dir_floor_up.png").c_str());
  textures_[9] = LoadTexture((dir_floor / "dir_floor_right.png").c_str());
  textures_[10] = LoadTexture((dir_floor / "dir_floor_down.png").c_str());
  textures_[11] = LoadTexture((dir_floor / "dir_floor_left.png").c_str());

  fs::path dir_box = dir / "directionals" / "dir_boxes";
  textures_[12] = LoadTexture((dir_box / "dir_box_up.png").c_str());
  textures_[13] = LoadTexture((dir_box / "dir_box_right.png").c_str());
  textures_[14] = LoadTexture((dir_box / "dir_box_down.png").c_str());
  textures_[15] = LoadTexture((dir_box / "dir_box_left.png").c_str());

  textures_[16] = LoadTexture((dir / "portal.png").c_str());
};