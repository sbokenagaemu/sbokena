#include "editor_level.hh"

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <raylib.h>

#include "level.hh"
#include "loader.hh"
#include "object.hh"
#include "position.hh"
#include "tile.hh"
#include "types.hh"
#include "utils.hh"

using nlohmann::json;
using namespace sbokena::position;
using namespace sbokena::editor::tile;
using namespace sbokena::editor::object;

namespace Common = sbokena::level;
namespace fs     = std::filesystem;

namespace sbokena::editor::level {

// resets the level back the empty default template but keep
// the name.
void Level::reset() {
  // clears all stored elements.
  tiles.map.clear();
  objects.map.clear();
  pos_tiles.clear();
  pos_objects.clear();
  linked_portals.clear();
  door_to_buttons.clear();
  button_to_door.clear();
  // resets the max id counters.
  tiles.max_id   = NULL_ID;
  objects.max_id = NULL_ID;
  // resets the condition.
  condition = Condition();
}

// ===== themes =====

namespace {
// internal helper; tries loading assets using given theme name.
bool try_load(
  std::string_view name, std::unique_ptr<Theme<Texture>> &output
) {
  output.reset();
  try {
    sbokena::loader::Theme<Image> temp(name, fs::current_path());
    output = std::make_unique<Theme<Texture>>(temp);
    return true;
  } catch (const sbokena::loader::ThemeLoadException) {
    output.reset();
    return false;
  }
}
} // namespace

// tries to load the assets needed for the current theme.
// if successful, returns 1 and uses the new theme/assets.
// else, tries to use the default theme/assets (dev), returns 0 if
// successfull.
// if all failed, returns -1, theme name is now NULL_NAME.
i32 Level::load_theme_assets() {
  // if loading requested theme successful, returns 1.
  if (try_load(theme_name, theme_assets))
    return 1;
  // else, try loading the default as a backup, then returns 0.
  if (try_load(DEFAULT_THEME_NAME, theme_assets)) {
    theme_name = DEFAULT_THEME_NAME;
    return 0;
  }
  theme_name = NULL_NAME;
  theme_assets.reset();
  return -1;
}

// fetches the tile at this position, depending on its type, returns
// an appropriate texture.
Texture Level::tile_sprite_at(Position<> pos) const {
  const auto &sprites = theme_assets->sprites();
  const auto  tile    = get_tile_at(pos);

  if (!tile)
    return *sprites[theme_assets->ROOF];

  switch (tile->get_type()) {
  case TileType::Floor:
    return *sprites[theme_assets->FLOOR];
    break;
  case TileType::Button:
    return *sprites[theme_assets->BUTTON];
    break;
  case TileType::Door: {
    const Door *door = dynamic_cast<const Door *>(tile);
    bool        open = door->is_opened();
    return *sprites
      [open ? theme_assets->DOOR_OPEN : theme_assets->DOOR_CLOSED];
    break;
  }
  case TileType::Portal: {
    const Portal *portal = dynamic_cast<const Portal *>(tile);
    Direction     dir    = portal->get_dir_in();
    switch (dir) {
    case Direction::Up:
      return *sprites[theme_assets->PORTAL_N];
      break;
    case Direction::Down:
      return *sprites[theme_assets->PORTAL_S];
      break;
    case Direction::Right:
      return *sprites[theme_assets->PORTAL_E];
      break;
    case Direction::Left:
      return *sprites[theme_assets->PORTAL_W];
      break;
    }
    break;
  }
  case TileType::OneDir: {
    const OneDir *onedir = dynamic_cast<const OneDir *>(tile);
    Direction     dir    = onedir->get_dir_in();
    switch (dir) {
    case Direction::Up:
      return *sprites[theme_assets->DIRFLOOR_N];
      break;
    case Direction::Down:
      return *sprites[theme_assets->DIRFLOOR_S];
      break;
    case Direction::Right:
      return *sprites[theme_assets->DIRFLOOR_E];
      break;
    case Direction::Left:
      return *sprites[theme_assets->DIRFLOOR_W];
    }
    break;
  }
  case TileType::Goal: {
    return *sprites[theme_assets->GOAL];
    break;
  }
  case TileType::Roof: {
    const Tile *tile_below = get_tile_at({pos.x, pos.y + 1});
    if (tile_below)
      return *sprites[theme_assets->WALL];
    else
      return *sprites[theme_assets->ROOF];
    break;
  }
  default:
    return *sprites[theme_assets->ROOF];
    break;
  }
}

// fetches the object at this position, if it exists.
// depending on its type, returns an appropriate texture.
std::optional<Texture> Level::object_sprite_at(Position<> pos) const {
  const auto &sprites = theme_assets->sprites();
  const auto  object  = get_object_at(pos);

  if (!object)
    return std::nullopt;

  switch (object->get_type()) {
  case ObjectType::Box:
    return *sprites[theme_assets->BOX];
    break;
  case ObjectType::OneDirBox: {
    const OneDirBox *box = dynamic_cast<const OneDirBox *>(object);
    switch (box->get_dir()) {
    case Direction::Up:
      return *sprites[theme_assets->DIRBOX_N];
      break;
    case Direction::Down:
      return *sprites[theme_assets->DIRBOX_S];
      break;
    case Direction::Right:
      return *sprites[theme_assets->DIRBOX_E];
      break;
    case Direction::Left:
      return *sprites[theme_assets->DIRBOX_W];
    }
    break;
  }
  case ObjectType::Player: {
    const Player *player = dynamic_cast<const Player *>(object);
    switch (player->get_dir()) {
    case Direction::Up:
      return *sprites[theme_assets->PLAYER_N];
      break;
    case Direction::Down:
      return *sprites[theme_assets->PLAYER_S];
      break;
    case Direction::Right:
      return *sprites[theme_assets->PLAYER_E];
      break;
    case Direction::Left:
      return *sprites[theme_assets->PLAYER_W];
    }
    break;
  }
  }
}

// ===== tiles =====

// creates a new tile, takes a type and a position as
// parameters.
u32 Level::create_tile(TileType type, const Position<> &pos) {
  u32 id;
  // if a tile already exists at the position, cannot create a new
  // tile.
  auto it = pos_tiles.find(pos);
  if (it != pos_tiles.end())
    return NULL_ID;
  // creates a new tile at the position.
  id = this->generate_tile_id();
  std::unique_ptr<Tile> tile;
  switch (type) {
  case TileType::Roof:
    tile = std::make_unique<Roof>(id);
    break;
  case TileType::Floor:
    tile = std::make_unique<Floor>(id);
    break;
  case TileType::OneDir:
    tile = std::make_unique<OneDir>(id);
    break;
  case TileType::Goal:
    tile = std::make_unique<Goal>(id);
    break;
  case TileType::Portal:
    tile = std::make_unique<Portal>(id);
    break;
  case TileType::Door:
    tile = std::make_unique<Door>(id);
    break;
  case TileType::Button:
    tile = std::make_unique<Button>(id);
    break;
  }

  pos_tiles[pos] = id;
  tiles.map[id]  = std::move(tile);
  return id;
}

// replace an old tile with a new one at a particular position.
// however, a new id would be used for the new tile.
u32 Level::replace_tile_at(const Position<> &pos, TileType type) {
  // if removing the old tile fails, this function also fails.
  if (remove_tile_at(pos) == false)
    return NULL_ID;
  // else remove and add a new tile normally.
  return create_tile(type, pos);
}

// removes a tile that doesn't have an object on.
bool Level::remove_tile(u32 id) {
  // checks if the tile actually exists.
  Tile *tile = get_tile(id);
  if (!tile)
    return false;
  // can’t remove tiles that have an object on them.
  if (tile->contains_obj())
    return false;
  // cleans up links if the tile is linked.
  switch (tile->get_type()) {
  case TileType::Portal: {
    (void)unlink_portal(id);
    break;
  }
  case TileType::Door: {
    (void)unlink_door(id);
    break;
  }
  case TileType::Button: {
    (void)unlink_button(id);
    break;
  }
  default:
    break;
  }
  // removes from pos_tiles (position -> id).
  for (auto it = pos_tiles.begin(); it != pos_tiles.end(); ++it) {
    if (it->second == id) {
      pos_tiles.erase(it);
      break;
    }
  }
  // removes from tile map.
  tiles.map.erase(id);
  return true;
}

// removes the tile at the specified position.
bool Level::remove_tile_at(const Position<> &pos) {
  auto it = pos_tiles.find(pos);
  // if the position contains no tile then cant remove tile.
  if (it == pos_tiles.end())
    return false;
  // else removes tile normally.
  return remove_tile(it->second);
}

// the pointer to the tile.
Tile *Level::get_tile(u32 id) {
  auto it = tiles.map.find(id);
  // if id doesn't exist return nullptr.
  if (it == tiles.map.end())
    return nullptr;
  // else returns the tile pointer.
  return it->second.get();
}

// the const pointer to the tile.
const Tile *Level::get_tile(u32 id) const {
  auto it = tiles.map.find(id);
  // if id doesn't exist return nullptr.
  if (it == tiles.map.end())
    return nullptr;
  // else returns the tile pointer.
  return it->second.get();
};

// returns the pointer to the tile at the specified
// position.
Tile *Level::get_tile_at(const Position<> &pos) {
  auto it = pos_tiles.find(pos);
  // if the position contains no tile then returns nullptr.
  if (it == pos_tiles.end())
    return nullptr;
  // else returns the tile pointer.
  return get_tile(it->second);
}

// returns the const pointer to the tile at the specified
// position.
const Tile *Level::get_tile_at(const Position<> &pos) const {
  auto it = pos_tiles.find(pos);
  // if the position contains no tile then returns nullptr.
  if (it == pos_tiles.end())
    return nullptr;
  // else returns the tile pointer.
  return get_tile(it->second);
}

// updates door state (whether it is opened or closed).
// is only opened if all of the linked buttons are
// activated. if contains an object, keeps its current
// state.
void Level::update_door_state(u32 id) {
  // if the door doesn't exist, or if there is an object on
  // it, does nothing.
  Door *door = dynamic_cast<Door *>(get_tile(id));
  if (!door || door->contains_obj())
    return;
  // if the door is unlinked, closes it.
  auto it = door_to_buttons.find(id);
  if (it == door_to_buttons.end() || it->second.empty()) {
    (void)door->close();
    return;
  }
  // checks whether all buttons exist and are pressed
  // (contain object).
  bool all_pressed = true;
  for (auto b_id : it->second) {
    Button *button = dynamic_cast<Button *>(get_tile(b_id));
    if (!button || !button->contains_obj()) {
      all_pressed = false;
      break;
    }
  }
  // if all buttons are pressed, opens the door, else closes
  // it.
  if (all_pressed)
    door->open();
  else
    door->close();
}

// ===== objects =====

// adds an object onto an existing tile.
u32 Level::add_object(ObjectType type, const Position<> &pos) {
  // if the position contains no tile then can't add object.
  if (!has_tile_at(pos))
    return NULL_ID;
  Tile *tile = get_tile_at(pos);
  // if the tile is a roof then can't add object.
  if (tile->get_type() == TileType::Roof)
    return NULL_ID;
  // if the tile already contains an object then can't add
  // object.
  if (tile->contains_obj())
    return NULL_ID;
  // else adds an object.
  u32                     id = this->generate_object_id();
  std::unique_ptr<Object> object;
  switch (type) {
  case ObjectType::Box:
    object = std::make_unique<Box>(id);
    break;
  case ObjectType::OneDirBox:
    object = std::make_unique<OneDirBox>(id);
    break;
  case ObjectType::Player:
    object = std::make_unique<Player>(id);
    break;
  }
  tile->set_obj_id(id);
  objects.map[id]  = std::move(object);
  pos_objects[pos] = id;

  // update linked door state if placed tile is a button or
  // a door.
  if (tile->get_type() == TileType::Button) {
    auto door_it = button_to_door.find(tile->get_id());
    if (door_it != button_to_door.end())
      update_door_state(door_it->second);
  } else if (tile->get_type() == TileType::Door) {
    update_door_state(tile->get_id());
  }

  return id;
}

// replace an old object with a new one at a particular position.
// however, a new id would be used for the new object.
u32 Level::replace_object_at(const Position<> &pos, ObjectType type) {
  // if removing the old object fails, this function also fails.
  Object *object = get_object_at(pos);
  if (!object)
    return NULL_ID;
  u32 object_id = object->get_id();
  if (object_id == NULL_ID)
    return NULL_ID;
  if (remove_object(object_id) == false)
    return NULL_ID;
  // else remove and add object normally.
  // can still fail if the position is invalid.
  return (add_object(type, pos));
}

// removes an object.
bool Level::remove_object(u32 id) {
  auto it = objects.map.find(id);
  // check if the object exists.
  if (it == objects.map.end())
    return false;
  // find current position of the object.
  Position<> pos;
  bool       found = false;
  for (auto &pair : pos_objects) {
    if (pair.second == id) {
      pos   = pair.first;
      found = true;
      break;
    }
  }
  if (!found)
    return false;
  // removes the object_id in the tile the object is on.
  Tile *cleanup = get_tile_at(pos);
  if (!cleanup)
    return false;
  cleanup->remove_obj_id();
  // if aforementioned tile is a button or a door, update the door
  // state.
  if (cleanup) {
    if (cleanup->get_type() == TileType::Button) {
      auto door_it = button_to_door.find(cleanup->get_id());
      if (door_it != button_to_door.end())
        update_door_state(door_it->second);
    } else if (cleanup->get_type() == TileType::Door) {
      update_door_state(cleanup->get_id());
    }
  }
  // removes from pos_objects (position -> id).
  pos_objects.erase(pos);
  // removes from object map.
  objects.map.erase(it);
  return true;
};

// the pointer to the object.
Object *Level::get_object(u32 id) {
  auto it = objects.map.find(id);
  if (it == objects.map.end())
    return nullptr;
  return it->second.get();
};

// the const pointer to the object.
const Object *Level::get_object(u32 id) const {
  auto it = objects.map.find(id);
  if (it == objects.map.end())
    return nullptr;
  return it->second.get();
}

// returns the pointer to the object at the specified
// position.
Object *Level::get_object_at(const Position<> &pos) {
  auto it = pos_objects.find(pos);
  // if the position contains no object then returns nullptr.
  if (it == pos_objects.end())
    return nullptr;
  // else returns the object pointer.
  return get_object(it->second);
};

// returns the const pointer to the object at the specified
// position.
const Object *Level::get_object_at(const Position<> &pos) const {
  auto it = pos_objects.find(pos);
  // if the position contains no object then returns nullptr.
  if (it == pos_objects.end())
    return nullptr;
  // else returns the object pointer.
  return get_object(it->second);
}

// moves the object to another existing position.
bool Level::move_object(u32 id, const Position<> &new_pos) {
  // checks if object exists.
  Object *object = get_object(id);
  if (!object)
    return false;
  // checks if the tile at the new position exists.
  Tile *tile = get_tile_at(new_pos);
  if (!tile)
    return false;
  // if tile is roof or contains an object, can't move
  // object there.
  if (tile->get_type() == TileType::Roof || tile->contains_obj())
    return false;
  // finds the current position of the object, if failed, returns
  // false.
  Position<> old_pos;
  bool       exists = false;
  for (auto &pair : pos_objects) {
    if (pair.second == id) {
      old_pos = pair.first;
      exists  = true;
      break;
    }
  }
  if (!exists)
    return false;
  // removes the object_id in the old tile.
  Tile *cleanup = get_tile_at(old_pos);
  if (!cleanup)
    return false;
  cleanup->remove_obj_id();
  // removes the old position entry in pos_objects.
  pos_objects.erase(old_pos);
  // places the object on a new tile and sets a new
  tile->set_obj_id(id);
  // adds new position entry in pos_objects.
  pos_objects[new_pos] = id;
  // update the state of possible linked doors in the old and new
  // tile.
  auto update = [&](Tile *t) {
    if (!t)
      return;
    if (t->get_type() == TileType::Button) {
      auto it = button_to_door.find(t->get_id());
      if (it != button_to_door.end())
        update_door_state(it->second);
    } else if (t->get_type() == TileType::Door) {
      update_door_state(t->get_id());
    }
    return;
  };
  update(cleanup);
  update(tile);
  return true;
}

// ===== portal pairs =====

// links two portals together.
bool Level::link_portals(u32 id1, u32 id2) {
  // checks if two ids are different.
  if (id1 == id2)
    return false;
  // checks if the portals exist.
  Portal *portal1 = dynamic_cast<Portal *>(get_tile(id1));
  Portal *portal2 = dynamic_cast<Portal *>(get_tile(id2));
  if (!portal1 || !portal2)
    return false;
  // both portals must be unlinked in order to link.
  if (portal1->is_linked() || portal2->is_linked())
    return false;
  // links portals.
  portal1->link(id2);
  portal2->link(id1);
  // add pair_id into both portals.
  u32 pair_id = generate_portal_pair_id();
  portal1->set_pair_id(pair_id);
  portal2->set_pair_id(pair_id);
  // adds the pair in both directions into linked_portals.
  linked_portals[id1] = id2;
  linked_portals[id2] = id1;
  return true;
}

// unlinks a portal, which also unlinks the other portal.
bool Level::unlink_portal(u32 portal_id) {
  auto it = linked_portals.find(portal_id);
  // checks if the portals pair exists.
  if (it == linked_portals.end())
    return false;
  // the other portal's id.
  u32  other_id = it->second;
  auto it2      = linked_portals.find(other_id);
  // checks if the reverse portal pair exists.
  if (it2 == linked_portals.end())
    return false;
  Portal *portal1 = dynamic_cast<Portal *>(get_tile(portal_id));
  Portal *portal2 = dynamic_cast<Portal *>(get_tile(other_id));
  if (!portal1 || !portal2)
    return false;
  // unlinks the portals.
  portal1->unlink();
  portal2->unlink();
  // removes the pair_id in both portals.
  portal1->clear_pair_id();
  portal2->clear_pair_id();
  // erases the two pairs in linked_portals.
  linked_portals.erase(portal_id);
  linked_portals.erase(other_id);
  return true;
}

// ===== door-button pairs =====

// links a door and a button together.
bool Level::link_door_button(u32 door_id, u32 button_id) {
  // checks if two ids are different.
  if (door_id == button_id)
    return false;
  // check if the door and button exist.
  Door   *door   = dynamic_cast<Door *>(get_tile(door_id));
  Button *button = dynamic_cast<Button *>(get_tile(button_id));
  if (!door || !button)
    return false;
  // if this button was linked before, unlink from its old
  // door
  auto it = button_to_door.find(button_id);
  if (it != button_to_door.end()) {
    if (!unlink_button(button_id))
      return false;
  }
  // links the door and the button.
  door->link(button_id);
  button->link(door_id);
  // adds the pair in both directions into door_to_button and
  // button_to_door.
  door_to_buttons[door_id].emplace(button_id);
  button_to_door[button_id] = door_id;
  // update door state after removing button.
  update_door_state(door_id);
  return true;
}

// unlinks the door, which also unlinks all linked buttons.
bool Level::unlink_door(u32 door_id) {
  // checks if the door to buttons pair exists.
  auto it = door_to_buttons.find(door_id);
  if (it == door_to_buttons.end())
    return false;
  // erases the button to door pairs.
  Door *door = dynamic_cast<Door *>(get_tile(door_id));
  if (!door)
    return false;
  for (auto button_id : it->second) {
    button_to_door.erase(button_id);
    Button *button = dynamic_cast<Button *>(get_tile(button_id));
    if (button)
      button->unlink();
  }
  // erases the door_to_buttons pair.
  door_to_buttons.erase(it);
  // closes the door and unlinks the door.
  door->unlink_all();
  door->close();
  return true;
}

// unlinks the button, which also unlinks the door from that
// button. if the door is now empty then closes it.
bool Level::unlink_button(u32 button_id) {
  // checks if the button to door pair exists.
  auto it = button_to_door.find(button_id);
  if (it == button_to_door.end())
    return false;
  // the other door's id.
  u32 door_id = it->second;
  // checks if the door and the button exists.
  Button *button = dynamic_cast<Button *>(get_tile(button_id));
  Door   *door   = dynamic_cast<Door *>(get_tile(door_id));
  if (!button || !door)
    return false;
  // erases the pairs in button_to_door and door_to_buttons.
  auto door_it = door_to_buttons.find(door_id);
  if (door_it == door_to_buttons.end())
    return false;
  door_it->second.erase(button_id);
  button_to_door.erase(button_id);
  // unlinks the button and door.
  button->unlink();
  door->unlink(button_id);
  // update door state after removing button.
  update_door_state(door_id);
  return true;
}

// ===== json =====

// checks whether the level is valid enough to be exported.
bool Level::is_valid() {
  // checks if the loaded texture is valid & matches the theme name.
  if (theme_assets == nullptr || theme_name == NULL_NAME
      || theme_name != theme_assets->name())
    return false;
  // checks if there is only one player.
  u32  player = 0;
  auto it     = objects.map.begin();
  while (it != objects.map.end()) {
    if (it->second->get_type() == ObjectType::Player)
      player++;
    it++;
  }
  if (player != 1)
    return false;
  // checks if all the doors, buttons, and portals are linked.
  // count the number of goals.
  u32  goals = 0;
  auto it2   = tiles.map.begin();
  while (it2 != tiles.map.end()) {
    switch (it2->second->get_type()) {
    case TileType::Door: {
      Door *door = dynamic_cast<Door *>(it2->second.get());
      if (!door || !door->is_linked())
        return false;
      break;
    }
    case TileType::Button: {
      Button *button = dynamic_cast<Button *>(it2->second.get());
      if (!button || !button->is_linked())
        return false;
      break;
    }
    case TileType::Portal: {
      Portal *portal = dynamic_cast<Portal *>(it2->second.get());
      if (!portal || !portal->is_linked())
        return false;
      break;
    }
    case TileType::Goal: {
      Goal *goal = dynamic_cast<Goal *>(it2->second.get());
      if (!goal)
        return false;
      goals++;
      break;
    }
    default:
      break;
    }
  }
  // checks if the no. boxes (unidirectional included) = no. of goals.
  u32 boxes = objects.map.size() - player;
  if (boxes != goals)
    return false;
  return true;
}

// tries to save the level at sbokena/levels as an .sbk file.
bool Level::save_file() {
  if (!is_valid())
    return false;

  std::map<Position<>, Common::Tile>   tiles;
  std::map<Position<>, Common::Object> objects;

  // convert editor tiles into raw tiles, put in a map.
  auto it = pos_tiles.begin();
  while (it != pos_tiles.end()) {
    auto       pos  = it->first;
    const auto tile = get_tile(it->second);
    switch (tile->get_type()) {
    case TileType::Roof:
      break;
    case TileType::Floor:
      tiles[pos] = Common::Floor {};
      break;
    case TileType::OneDir: {
      const auto *onedir = dynamic_cast<OneDir *>(tile);
      tiles[pos]         = Common::DirFloor {onedir->get_dir_in()};
      break;
    }
    case TileType::Goal:
      tiles[pos] = Common::Goal {};
      break;
    case TileType::Portal: {
      const auto *portal = dynamic_cast<Portal *>(tile);
      tiles[pos] =
        Common::Portal {portal->get_pair_id(), portal->get_dir_in()};
      break;
    }
    case TileType::Button: {
      const auto *button = dynamic_cast<Button *>(tile);
      tiles[pos]         = Common::Button {button->get_linked()};
      break;
    }
    case TileType::Door: {
      tiles[pos] = Common::Door {it->second};
      break;
    }
    }
  }

  // convert editor objects into raw objects, put in a map.
  auto it2 = pos_objects.begin();
  while (it2 != pos_objects.end()) {
    auto       pos    = it2->first;
    const auto object = get_object(it2->second);
    switch (object->get_type()) {
    case ObjectType::Player:
      objects[pos] = Common::Player {};
      break;
    case ObjectType::Box:
      objects[pos] = Common::Box {};
      break;
    case ObjectType::OneDirBox: {
      const auto dirbox = dynamic_cast<OneDirBox *>(object);
      objects[pos]      = Common::DirBox {dirbox->get_dir()};
      break;
    }
    }
    it2++;
  }

  u32 diff_int            = static_cast<u32>(condition.difficulty);
  Common::Difficulty diff = static_cast<Common::Difficulty>(diff_int);

  Common::RawLevel raw_level {name, theme_name, diff, tiles, objects};

  nlohmann::json j;
  to_json(j, raw_level);

  auto selected = sbokena::utils::save_file_dialog();
  if (!selected.has_value())
    return false;
  fs::path save_path = selected.value();

  if (save_path.extension() != ".sbk")
    save_path.replace_extension(".sbk");

  try {
    std::ofstream file(save_path);
    if (!file.is_open())
      return false;
    file << j.dump(2);
    file.close();
  } catch (...) {
    return false;
  }
  return true;
}

} // namespace sbokena::editor::level
