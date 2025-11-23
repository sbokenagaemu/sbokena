#include "editor_level.hh"
#include "types.hh"

#include <memory>
#include <unordered_map>

using namespace sbokena::position;

// resets the level back the empty default template but keep the name.
void Level::reset() {
  // clears all stored elements.
  tiles.map.clear();
  objects.map.clear();
  positions.clear();
  linked_portals.clear();
  door_to_button.clear();
  button_to_door.clear();
  // resets the max id counters.
  tiles.max_id = null_id;
  objects.max_id = null_id;
  // resets the condition.
  condition = Condition();
}

// Functions regarding tiles.

// creates a new tile, takes a type and a position as parameters.
u32 Level::create_tile(TileType type, const Position<> &pos) {
  u32 id;
  // if a tile already exists at the position, cannot create a new tile.
  auto it = positions.find(pos);
  if (it != positions.end()) {
    return null_id;
  }
  // creates a new tile at the position.
  id = this->generate_tile_id();
  std::unique_ptr<Tile> tile;
  switch (type) {
  case TileType::Wall:
    tile = std::make_unique<Wall>(id);
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

  positions[pos] = id;
  tiles.map[id] = std::move(tile);
  return id;
}

// removes a tile that doesn't have an object on.
bool Level::remove_tile(u32 id) {
  // checks if the tile actually exists.
  Tile *tile = get_tile(id);
  if (!tile)
    return false;
  // can’t remove tiles that have an object on them.
  if (tile->contains_obj()) {
    return false;
  }
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
  // removes from positions (position -> id).
  for (auto it = positions.begin(); it != positions.end(); ++it) {
    if (it->second == id) {
      positions.erase(it);
      break;
    }
  }
  // removes from tile map.
  tiles.map.erase(id);
  return true;
}

// removes the tile at the specified position.
bool Level::remove_tile_at(const Position<> &pos) {
  auto it = positions.find(pos);
  // if the position contains no tile then cant remove tile.
  if (it == positions.end())
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

// returns the pointer to the tile at the specified position.
Tile *Level::get_tile_at(const Position<> &pos) {
  auto it = positions.find(pos);
  // if the position contains no tile then returns nullptr.
  if (it == positions.end())
    return nullptr;
  // else returns the tile pointer.
  return get_tile(it->second);
}

// returns the const pointer to the tile at the specified position.
const Tile *Level::get_tile_at(const Position<> &pos) const {
  auto it = positions.find(pos);
  // if the position contains no tile then returns nullptr.
  if (it == positions.end())
    return nullptr;
  // else returns the tile pointer.
  return get_tile(it->second);
}

// Functions regarding objects.

// adds an object onto an existing tile.
u32 Level::add_object(ObjectType type, const Position<> &pos) {
  // if the position contains no tile then can't add object.
  if (!has_tile_at(pos))
    return null_id;
  Tile *tile = get_tile_at(pos);
  // if the tile is a wall then can't add object.
  if (tile->get_type() == TileType::Wall)
    return null_id;
  // if the tile already contains an object then can't add object.
  if (tile->contains_obj())
    return null_id;
  // else adds an object.
  u32 id = this->generate_object_id();
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
  objects.map[id] = std::move(object);
  return id;
}

// removes an object.
bool Level::remove_object(u32 id) {
  auto it = objects.map.find(id);
  // check if the object exists.
  if (it == objects.map.end())
    return false;
  // removes the object_id in the tile the object is on.
  for (auto &pair : positions) {
    u32 tile_id = pair.second;
    Tile *tile = get_tile(tile_id);
    if (tile) {
      if (tile->contains_obj() && tile->get_obj_id() == id) {
        tile->remove_obj_id();
        break;
      }
    }
  }
  // removes the object.
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

// returns the pointer to the object at the specified position.
Object *Level::get_object_at(const Position<> &pos) {
  Tile *tile = get_tile_at(pos);
  if (!tile)
    return nullptr;
  if (!tile->contains_obj())
    return nullptr;
  u32 id = tile->get_obj_id();
  return get_object(id);
};

// returns the const pointer to the object at the specified position.
const Object *Level::get_object_at(const Position<> &pos) const {
  const Tile *tile = get_tile_at(pos);
  if (!tile)
    return nullptr;
  if (!tile->contains_obj())
    return nullptr;
  u32 id = tile->get_obj_id();
  return get_object(id);
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
  // if tile is wall or contains an object, can't move object there.
  if (tile->get_type() == TileType::Wall || tile->contains_obj())
    return false;
  // removes the object_id in the tile the object is currently on.
  for (auto &pair : positions) {
    u32 tile_id = pair.second;
    Tile *tile = get_tile(tile_id);
    if (tile) {
      if (tile->contains_obj() && tile->get_obj_id() == id) {
        tile->remove_obj_id();
        break;
      }
    }
  }
  // places the object on a new tile.
  tile->set_obj_id(id);
  return true;
}

// Functions regarding portal pairs.

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
  portal1->link();
  portal2->link();
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
  u32 other_id = it->second;
  auto it2 = linked_portals.find(other_id);
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
  // erases the two pairs in linked_portals.
  linked_portals.erase(portal_id);
  linked_portals.erase(other_id);
  return true;
}

// Functions regarding door-button pairs.

// links a door and a button together.
bool Level::link_door_button(u32 door_id, u32 button_id) {
  // checks if two ids are different.
  if (door_id == button_id)
    return false;
  // checks if the tiles exist.
  Tile *tile1 = get_tile(door_id);
  Tile *tile2 = get_tile(button_id);
  if (!tile1 || !tile2)
    return false;
  // checks if the 1st param is a door and the 2nd param is a button.
  if (tile1->get_type() != TileType::Door ||
      tile2->get_type() != TileType::Button)
    return false;
  Door *door = dynamic_cast<Door *>(tile1);
  Button *button = dynamic_cast<Button *>(tile2);
  if (door->is_linked() || button->is_linked())
    return false;
  // links the door and the button.
  door->link();
  button->link();
  // adds the pair in both directions into door_to_button and button_to_door.
  door_to_button[door_id] = button_id;
  button_to_door[button_id] = door_id;
  return true;
}

// unlinks the door, which also unlinks the button.
bool Level::unlink_door(u32 door_id) {
  auto it = door_to_button.find(door_id);
  // checks if the door to button pair exists.
  if (it == door_to_button.end())
    return false;
  // the other button's id.
  u32 button_id = it->second;
  auto it2 = button_to_door.find(button_id);
  // checks if the button to door pair exists.
  if (it2 == button_to_door.end())
    return false;
  Door *door = dynamic_cast<Door *>(get_tile(door_id));
  Button *button = dynamic_cast<Button *>(get_tile(button_id));
  if (!door || !button)
    return false;
  // closes the door, unlink the door and the button.
  door->unlink();
  door->close();
  button->unlink();
  // erases the pairs in door_to_button and button_to_door.
  door_to_button.erase(door_id);
  button_to_door.erase(button_id);
  return true;
}

// unlinks the button, which also unlinks the door.
bool Level::unlink_button(u32 button_id) {
  auto it = button_to_door.find(button_id);
  // checks if the button to door pair exists.
  if (it == button_to_door.end())
    return false;
  // the other door's id.
  u32 door_id = it->second;
  auto it2 = door_to_button.find(door_id);
  // checks if the door to button pair exists.
  if (it2 == door_to_button.end())
    return false;
  Button *button = dynamic_cast<Button *>(get_tile(button_id));
  Door *door = dynamic_cast<Door *>(get_tile(door_id));
  if (!button || !door)
    return false;
  // closes the door, unlink the button and the door.
  button->unlink();
  door->unlink();
  door->close();
  // erases the pairs in button_to_door and door_to_button.
  button_to_door.erase(button_id);
  door_to_button.erase(door_id);
  return true;
}