#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif

#include <Color.hpp>
#include <Window.hpp>
#include <raygui.h>
#include <raylib.h>

#include "editor_level.hh"
#include "grid.hh"

using namespace sbokena::types;
using namespace sbokena::editor::level;
using namespace sbokena::editor::tile;

// for testing purposes
#include <iostream>

constexpr u32 thickness_coef               = 12;
constexpr u32 taskbar_button_size          = 40;
constexpr u32 tile_picker_padding          = 10;
constexpr u32 tile_picker_box_size         = 80;
constexpr u32 tile_picker_box_padding      = 25;
constexpr u32 tile_picker_space_inbetween  = 20;
constexpr u32 view_control_space_inbetween = 20;
constexpr u32 view_control_button_width    = 60;
constexpr u32 view_control_button_height   = 50;
constexpr u32 dropdown_width               = 100;
constexpr u32 grid_view_min_height         = 45;
constexpr u32 view_control_padding         = tile_picker_padding + 5;
// also the minimum width of the grid-view
constexpr u32 tile_picker_width = 250;
constexpr u32 tile_first_col_x =
  tile_picker_padding + tile_picker_box_padding;
constexpr u32 tile_picker_delta =
  tile_picker_box_size + tile_picker_space_inbetween;
constexpr f32 min_width  = 550;
constexpr f32 min_height = 530;

bool edit_mode_theme = false;
char input[128]      = "";

f32             current_tile_size = 64;
raylib::Vector2 mouse_position;
raylib::Vector2 mouse_start_position;
raylib::Vector2 grid_offset              = {0, 0};
raylib::Vector2 selected_tile_position   = {0, 0};
Position<>      selected_grid_tile_index = {0, 0};
Position<>      link_selection_index     = {0, 0};

bool       is_placing_tiles             = true;
TileType   currently_selected_tile_type = TileType::Roof;
ObjectType currently_selected_object_type;
Rectangle  currently_selected_outline_rec = {
  tile_first_col_x,
  tile_picker_box_padding,
  tile_picker_box_size,
  tile_picker_box_size
};

float     thickness = 0;
Rectangle link_selection;
bool      window_exit = false;

bool select_same = false;
enum class Selecting { None, Tile, Object };
Selecting layer = Selecting::None;

void search_theme(Level &lvl, std::string &input) {
  std::string current_name = lvl.get_theme_name();
  lvl.change_theme(input);
  switch (lvl.load_theme_assets()) {
  case 1: {
    break;
  }
  case 0: {
    input = "";
    lvl.change_theme(current_name);
    break;
  }
  case -1: {
    window_exit = true;
    break;
  }
  }
}

void select_advance() {
  switch (layer) {
  case Selecting::None: {
    layer = Selecting::Tile;
    break;
  }
  case Selecting::Tile: {
    layer = Selecting::Object;
    break;
  }
  case Selecting::Object: {
    layer = Selecting::Tile;
  }
  }
}

enum class Edit_Mode { Place, Select, Link };
Edit_Mode mode = Edit_Mode::Select;

// switches between directional and normal objects (Boxes)
void switch_object(Level &lvl, Position<> pos) {
  Object *obj = lvl.get_object_at(pos);
  if (obj) {
    ObjectType type = obj->get_type();

    switch (type) {
    case ObjectType::Box: {
      lvl.replace_object_at(pos, ObjectType::OneDirBox);
      break;
    }
    case ObjectType::OneDirBox: {
      lvl.replace_object_at(pos, ObjectType::Box);
      break;
    }
    default:
      break;
    }
  }
}

// switches between directional and normal tiles (Floors)
void switch_tile(Level &lvl, Position<> pos) {
  Tile *tile = lvl.get_tile_at(pos);
  if (tile) {
    TileType type = tile->get_type();

    switch (type) {
    case TileType::Floor: {
      lvl.replace_tile_at(pos, TileType::OneDir);
      break;
    }
    case TileType::OneDir: {
      lvl.replace_tile_at(pos, TileType::Floor);
      break;
    }
    default:
      break;
    }
  }
}

void switch_selection(Rectangle rec, bool is_tile) {
  currently_selected_outline_rec = {
    rec.x - 5,
    rec.y - 5,
    tile_picker_box_size + 10,
    tile_picker_box_size + 10
  };
  is_placing_tiles = is_tile;
  mode             = Edit_Mode::Place;
  layer            = Selecting::None;
}

int main() {
  raylib::Color color_;

  raylib::Window window(
    min_width,
    min_height,
    "Window",
    FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE
  );

  SetWindowMinSize(min_width, min_height);

  Level level_       = Level("default");
  int   theme_result = level_.load_theme_assets();

  if (theme_result < 0)
    std::cerr << "Failed to load theme assets\n";

  while (!window.ShouldClose() && !window_exit) {
    window.BeginDrawing();
    mouse_position = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
      mouse_start_position = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
      raylib::Vector2 difference =
        Vector2Subtract(mouse_position, mouse_start_position);
      grid_offset += difference;
      selected_tile_position += difference;
      mouse_start_position = GetMousePosition();
    }

    // Clear gray background
    window.ClearBackground(raylib::Color::LightGray());

    // resizes the window to be at least it's minimum
    f32 current_window_width  = window.GetWidth();
    f32 current_window_height = window.GetHeight();

    if (current_window_width < min_width)
      current_window_width = min_width;

    if (current_window_height < min_height)
      current_window_height = min_height;

    // tile selection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      // checks whether:
      // 1) the mouse is inside the window's grid part
      // 2) the mouse is inside the actual grid
      if (is_inside(
            mouse_position,
            {tile_picker_width, grid_view_min_height},
            {current_window_width, current_window_height}
          )
          && is_inside(
            mouse_position,
            grid_offset,
            grid_end(grid_offset, current_tile_size)
          )) {
        Position<> next_selected_grid_tile_index =
          tile_index(mouse_position, grid_offset, current_tile_size);
        select_same =
          (next_selected_grid_tile_index == selected_grid_tile_index);
        selected_grid_tile_index = next_selected_grid_tile_index;
        switch (mode) {
        case (Edit_Mode::Place): {
          if (is_placing_tiles) {
            Tile *tile =
              level_.get_tile_at(next_selected_grid_tile_index);
            if (tile) {
              if (currently_selected_tile_type == TileType::Roof) {
                level_.remove_tile_at(next_selected_grid_tile_index);
              } else {
                level_.replace_tile_at(
                  selected_grid_tile_index,
                  currently_selected_tile_type
                );
              }
            } else if (currently_selected_tile_type
                       != TileType::Roof) {
              level_.create_tile(
                currently_selected_tile_type, selected_grid_tile_index
              );
            }

          } else {
            Object *obj =
              level_.get_object_at(next_selected_grid_tile_index);
            if (obj) {
              level_.replace_object_at(
                selected_grid_tile_index,
                currently_selected_object_type
              );
            } else {
              level_.add_object(
                currently_selected_object_type,
                selected_grid_tile_index
              );
            }
          }
          break;
        }
        case (Edit_Mode::Select): {
          selected_tile_position = raylib::Vector2 {
            grid_offset.GetX()
              + selected_grid_tile_index.x * current_tile_size,
            grid_offset.GetY()
              + selected_grid_tile_index.y * current_tile_size
          };

          if (select_same)
            select_advance();
          else
            layer = Selecting::Tile;
          break;
        }
        case (Edit_Mode::Link): {
          Tile *first_tile = level_.get_tile_at(link_selection_index);
          Tile *second_tile =
            level_.get_tile_at(selected_grid_tile_index);
          if (first_tile && second_tile) {
            // linking 2 portals together
            if (Portal *first = dynamic_cast<Portal *>(first_tile)) {
              if (Portal *second =
                    dynamic_cast<Portal *>(second_tile)) {
                level_.link_portals(
                  first->get_id(), second->get_id()
                );
              }
            }

            // linking a button to a door
            if (Button *first = dynamic_cast<Button *>(first_tile)) {
              if (Door *second = dynamic_cast<Door *>(second_tile)) {
                level_.link_door_button(
                  second->get_id(), first->get_id()
                );
              }
            }

            // linking a door to a button
            if (Door *first = dynamic_cast<Door *>(first_tile)) {
              if (Button *second =
                    dynamic_cast<Button *>(second_tile)) {
                level_.link_door_button(
                  first->get_id(), second->get_id()
                );
              }
            }
          }
          mode = Edit_Mode::Select;
          break;
        }
        }
      }
    }

    // GRID of Tiles
    for (u32 y = 0; y < 31; y++) {
      for (u32 x = 0; x < 31; x++) {
        DrawTextureEx(
          level_.tile_sprite_at({x, y}),
          {grid_offset.GetX() + current_tile_size * x,
           grid_offset.GetY() + current_tile_size * y},
          0,
          current_tile_size / 32,
          raylib::Color::White()
        );
      }
    }

    // selection
    const Rectangle grid_selection = {
      selected_tile_position.GetX(),
      selected_tile_position.GetY(),
      current_tile_size,
      current_tile_size
    };
    // Selecting::Tile
    if (layer == Selecting::Tile) {
      DrawRectangleRec(
        grid_selection, Fade(raylib::Color::Gray(), 0.5)
      );
    }

    // GRID of Objects
    for (u32 y = 0; y < 31; y++) {
      for (u32 x = 0; x < 31; x++) {
        auto object = level_.object_sprite_at({x, y});
        if (object.has_value()) {
          DrawTextureEx(
            object.value(),
            {grid_offset.GetX() + current_tile_size * x,
             grid_offset.GetY() + current_tile_size * y},
            0,
            current_tile_size / 32,
            raylib::Color::White()
          );
        }
      }
    }

    // Selecting::Object
    if (layer == Selecting::Object) {
      DrawRectangleRec(
        grid_selection, Fade(raylib::Color::Gray(), 0.5)
      );
    }

    // deselecting the tile
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
      layer = Selecting::None;

    // showing the link selection
    if (mode == Edit_Mode::Link) {
      DrawRectangleLinesEx(
        link_selection, thickness, raylib::Color::DarkPurple()
      );
    }

    // DECORATIVE ELEMENTS
    // tile_picker
    const Rectangle tile_picker = {
      0, 0, tile_picker_width, current_window_height
    };
    DrawRectangleRec(tile_picker, raylib::Color::Gray());

    // tile_picker_vertical_1
    const Rectangle tile_picker_vertical_1 = {
      0, 0, tile_picker_padding, current_window_height
    };
    DrawRectangleRec(tile_picker_vertical_1, raylib::Color::Black());

    // tile_picker_vertical_2
    const Rectangle tile_picker_vertical_2 = {
      tile_picker_width - tile_picker_padding,
      0,
      tile_picker_padding,
      current_window_height
    };
    DrawRectangleRec(tile_picker_vertical_2, raylib::Color::Black());

    // taskbar
    const Rectangle taskbar = {
      tile_picker_width,
      0,
      current_window_width - tile_picker_width,
      taskbar_button_size
    };
    DrawRectangleRec(taskbar, raylib::Color::Gray());

    // taskbar_line
    const Rectangle taskbar_line = {
      tile_picker_width,
      taskbar_button_size,
      current_window_width - tile_picker_width,
      5
    };
    DrawRectangleRec(taskbar_line, raylib::Color::Black());

    // view_control_rec
    const Rectangle view_control_rec = {
      tile_picker_padding,
      current_window_height - (2 * view_control_button_height + 10),
      tile_picker_width - 2 * tile_picker_padding,
      2 * view_control_button_height + 10
    };
    DrawRectangleRec(view_control_rec, raylib::Color::Black());

    // BUTTONS
    // Exit button
    const Rectangle exit_button = {
      current_window_width - taskbar_button_size,
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(exit_button, "Exit"))
      window_exit = true;

    // Load/Import button
    const Rectangle load_button = {
      current_window_width - (2 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(load_button, "Load"))
      level_.load_file();

    // Save/Export button
    const Rectangle save_button = {
      current_window_width - (3 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(save_button, "Save"))
      level_.save_file();

    // Reset button
    const Rectangle reset_button = {
      current_window_width - (4 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(reset_button, "Reset")) {
      level_.reset();
      theme_result = level_.load_theme_assets();
    }

    // Theme Textbox
    const Rectangle textbox = {
      tile_picker_width + 20,
      5,
      dropdown_width,
      taskbar_button_size - 10
    };
    DrawRectangleRec(textbox, raylib::Color::Black());
    if (GuiTextBox(textbox, input, 10, edit_mode_theme))
      edit_mode_theme = true;
    if (edit_mode_theme && IsKeyPressed(KEY_ENTER)) {
      std::string in = input;
      search_theme(level_, in);
      edit_mode_theme = false;
    }

    // Zoom in button
    const Rectangle zoom_in_button = {
      view_control_padding,
      current_window_height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(zoom_in_button, "Zoom in")) {
      if (current_tile_size <= 256) {
        current_tile_size *= 2;
        grid_offset = refactored_grid_offset(
          grid_offset, selected_tile_position, 2
        );
        selected_tile_position = raylib::Vector2 {
          grid_offset.GetX()
            + selected_grid_tile_index.x * current_tile_size,
          grid_offset.GetY()
            + selected_grid_tile_index.y * current_tile_size
        };
      }
    }

    // Rotate button
    const Rectangle rotate_button = {
      (view_control_padding + view_control_space_inbetween)
        + view_control_button_width,
      current_window_height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(rotate_button, "Rotate")) {
      if (mode == Edit_Mode::Select) {
        switch (layer) {
        case Selecting::Tile: {
          Tile *tile_ = level_.get_tile_at(selected_grid_tile_index);
          if (tile_) {
            if (OneDir *one_dir_ = dynamic_cast<OneDir *>(tile_))
              one_dir_->rotate();
            else if (Portal *portal_ = dynamic_cast<Portal *>(tile_))
              portal_->rotate();
          }
          break;
        }
        case Selecting::Object: {
          Object *object_ =
            level_.get_object_at(selected_grid_tile_index);
          if (object_) {
            if (OneDirBox *one_dir_box_ =
                  dynamic_cast<OneDirBox *>(object_))
              one_dir_box_->rotate();
          }
          break;
        }
        default:
          break;
        }
      }
    }

    // Zoom out button
    const Rectangle zoom_out_button = {
      (view_control_padding + 2 * view_control_space_inbetween)
        + (2 * view_control_button_width),
      current_window_height - view_control_button_height,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(zoom_out_button, "Zoom out")) {
      if (current_tile_size >= 32) {
        current_tile_size /= 2;
        grid_offset = refactored_grid_offset(
          grid_offset, selected_tile_position, 0.5
        );
        selected_tile_position = raylib::Vector2 {
          grid_offset.GetX()
            + selected_grid_tile_index.x * current_tile_size,
          grid_offset.GetY()
            + selected_grid_tile_index.y * current_tile_size
        };
      }
    }

    // Link button
    const Rectangle link_button = {
      view_control_padding,
      current_window_height - 2 * view_control_button_height - 5,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(link_button, "Link")) {
      thickness            = current_tile_size / thickness_coef;
      link_selection_index = selected_grid_tile_index;
      link_selection       = {
        selected_tile_position.GetX() - (thickness / 2),
        selected_tile_position.GetY() - (thickness / 2),
        current_tile_size + thickness,
        current_tile_size + thickness
      };
      mode = Edit_Mode::Link;
    };

    // Deselect button
    const Rectangle deselect_button = {
      (view_control_padding + view_control_space_inbetween)
        + view_control_button_width,
      current_window_height - 2 * view_control_button_height - 5,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(deselect_button, "Deselect"))
      mode = Edit_Mode::Select;

    // Switch button (?)
    const Rectangle switch_button = {
      (view_control_padding + 2 * view_control_space_inbetween)
        + (2 * view_control_button_width),
      current_window_height - 2 * view_control_button_height - 5,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(switch_button, "Switch")) {
      mode = Edit_Mode::Select;
      switch (layer) {
      case Selecting::Tile: {
        switch_tile(level_, selected_grid_tile_index);
        break;
      }
      case Selecting::Object: {
        switch_object(level_, selected_grid_tile_index);
        break;
      }
      default:
        break;
      }
    }

    // TILES
    // Selection outline
    if (mode == Edit_Mode::Place) {
      DrawRectangleRec(
        currently_selected_outline_rec, raylib::Color::Beige()
      );
    }

    // Floor tile
    const Rectangle floor_tile = {
      tile_first_col_x,
      tile_picker_box_padding,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(floor_tile, "Floor")) {
      currently_selected_tile_type = TileType::Floor;
      switch_selection(floor_tile, true);
    }
    // DrawRectangleRec(floor_tile, raylib::Color::Red());

    // Roof tile
    const Rectangle roof_tile = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(roof_tile, "Roof")) {
      currently_selected_tile_type = TileType::Roof;
      switch_selection(roof_tile, true);
    }
    // DrawRectangleRec(roof_tile, raylib::Color::Red());

    // Player tile
    const Rectangle player_object = {
      tile_first_col_x,
      tile_picker_box_padding + tile_picker_delta,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(player_object, "Player")) {
      currently_selected_object_type = ObjectType::Player;
      switch_selection(player_object, false);
    }
    // DrawRectangleRec(player_tile, raylib::Color::Red());

    // Goal tile
    const Rectangle goal_tile = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding + tile_picker_delta,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(goal_tile, "Goal")) {
      currently_selected_tile_type = TileType::Goal;
      switch_selection(goal_tile, true);
    }
    // DrawRectangleRec(goal_tile, raylib::Color::Red());

    // Button tile
    const Rectangle button_tile = {
      tile_first_col_x,
      tile_picker_box_padding + tile_picker_delta * 2,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(button_tile, "Button")) {
      currently_selected_tile_type = TileType::Button;
      switch_selection(button_tile, true);
    }
    // DrawRectangleRec(button_tile, raylib::Color::Red());

    // Door tile
    const Rectangle door_tile = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding + tile_picker_delta * 2,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(door_tile, "Door")) {
      currently_selected_tile_type = TileType::Door;
      switch_selection(door_tile, true);
    }
    // DrawRectangleRec(door_tile, raylib::Color::Red());

    // Portal tile
    const Rectangle portal_tile = {
      tile_first_col_x,
      tile_picker_box_padding + tile_picker_delta * 3,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(portal_tile, "Portal")) {
      currently_selected_tile_type = TileType::Portal;
      switch_selection(portal_tile, true);
    }
    // DrawRectangleRec(portal_tile, raylib::Color::Red());

    // Box tile
    const Rectangle box_object = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding + tile_picker_delta * 3,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(box_object, "Box")) {
      currently_selected_object_type = ObjectType::Box;
      switch_selection(box_object, false);
    }
    // DrawRectangleRec(box_tile, raylib::Color::Red());

    window.EndDrawing();
  }

  return 0;
}
