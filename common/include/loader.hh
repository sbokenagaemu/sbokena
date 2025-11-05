// texture loader

#pragma once

#include <raylib.h>
#include <vector>

// indexed as the following) (for now)
// 0  - roof (missing)
// 1  - wall
// 2  - floor
// 3  - player
// 4  - crate
// 5  - button (missing)
// 6  - door (open)
// 7  - door (closed)
// 8  - directional floor up
// 9  - directional floor right
// 10 - directional floor down
// 11 - directional floor left
// 12 - directional crate up
// 13 - directional crate right
// 14 - directional crate down
// 15 - directional crate left
// 16 - portal 1
// 17 - portal 2
// 18 - portal 3

// themes_ vectors contain all textures required for a single theme (they are
class TextureID {
public:
  TextureID();
  std::vector<Image> GetTextures(int id);

private:
  void load_textures();
  int theme_count_;
  std::vector<std::vector<Image>> themes_;
};