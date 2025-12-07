#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include <Vector2.hpp>

#include "object.hh"
#include "position.hh"
#include "tile.hh"
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

const char     *options_theme     = "Theme #1;Theme #2;Theme #3";
int             active_theme      = 0;
bool            edit_mode_theme   = false;
f32             current_tile_size = 64;
raylib::Vector2 mouse_position;
raylib::Vector2 mouse_start_position;
raylib::Vector2 grid_offset              = {0, 0};
raylib::Vector2 selected_tile_position   = {0, 0};
Position<>      selected_grid_tile_index = {0, 0};
bool            is_selection_shown       = false;

TileType   currently_selected_tile_type = TileType::Roof;
ObjectType currently_selected_object_type;
bool       is_currently_placing           = false;
Rectangle  currently_selected_outline_rec = {
  tile_first_col_x,
  tile_picker_box_padding,
  tile_picker_box_size,
  tile_picker_box_size
};

void switch_selection(Rectangle rec) {
  currently_selected_outline_rec = {
    rec.x - 5,
    rec.y - 5,
    tile_picker_box_size + 10,
    tile_picker_box_size + 10
  };
  is_currently_placing = true;
  is_selection_shown   = false;
}

int main() {
  Level         level_ = Level("default");
  raylib::Color color_;

  raylib::Window window(
    min_width,
    min_height,
    "Window",
    FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE
  );
  bool exit = false;

  SetWindowMinSize(min_width, min_height);

  while (!window.ShouldClose() && !exit) {
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

    // GRID
    for (u32 y = 0; y < 31; y++) {
      for (u32 x = 0; x < 31; x++) {
        const Rectangle tile = {
          grid_offset.GetX() + current_tile_size * x,
          grid_offset.GetY() + current_tile_size * y,
          current_tile_size,
          current_tile_size
        };
        if ((x + y) % 2)
          color_ = raylib::Color::Black();
        else
          color_ = raylib::Color::White();
        DrawRectangleRec(tile, color_);
      }
    }

    // tile selection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
        && !is_currently_placing) {
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
        selected_grid_tile_index =
          tile_index(mouse_position, grid_offset, current_tile_size);

        std::cout << "yes" << std::endl;
        selected_tile_position = (raylib::Vector2) {
          grid_offset.GetX()
            + selected_grid_tile_index.x * current_tile_size,
          grid_offset.GetY()
            + selected_grid_tile_index.y * current_tile_size
        };

        is_selection_shown = true;
      }
    }

    // deselecting the tile
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
      is_selection_shown = false;

    // showing the selection
    if (is_selection_shown) {
      const Rectangle grid_selection = {
        selected_tile_position.GetX(),
        selected_tile_position.GetY(),
        current_tile_size,
        current_tile_size
      };
      DrawRectangleRec(
        grid_selection, Fade(raylib::Color::Gray(), 0.5)
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
      exit = true;

    // Download button
    const Rectangle download_button = {
      current_window_width - (2 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(download_button, "Download")) {
      // TODO
    }

    // Import button
    const Rectangle import_button = {
      current_window_width - (3 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(import_button, "Import")) {
      // TODO
    }

    // Reset button
    const Rectangle reset_button = {
      current_window_width - (4 * taskbar_button_size),
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(reset_button, "Reset"))
      level_.reset();

    // Theme Dropdown menu
    const Rectangle dropdown_theme = {
      tile_picker_width + 20,
      5,
      dropdown_width,
      taskbar_button_size - 10
    };
    if (GuiDropdownBox(
          dropdown_theme,
          options_theme,
          &active_theme,
          edit_mode_theme
        )) {
      edit_mode_theme = !edit_mode_theme;
    }

    /**
    // Undo button (?)
    const Rectangle undo_button = {
      tile_picker_width, 0, taskbar_button_size, taskbar_button_size
    };
    if (GuiButton(undo_button, "Undo")) {
      // TODO
    }

    // Redo button (?)
    const Rectangle redo_button = {
      tile_picker_width + taskbar_button_size,
      0,
      taskbar_button_size,
      taskbar_button_size
    };
    if (GuiButton(redo_button, "Redo")) {
      // TODO
    }
    */

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
        selected_tile_position = grid_offset.Add(
          selected_grid_tile_index.Scale(current_tile_size)
        );
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
      if (is_selection_shown && !is_currently_placing) {
        Tile *tile_ = level_.get_tile_at(selected_grid_tile_index);
        if (tile_) {
          if (OneDir *one_dir_ = dynamic_cast<OneDir *>(tile_))
            one_dir_->rotate();
          else if (Portal *portal_ = dynamic_cast<Portal *>(tile_))
            portal_->rotate();
        }
        Object *object_ =
          level_.get_object_at(selected_grid_tile_index);
        if (object_) {
          if (OneDirBox *one_dir_box_ =
                dynamic_cast<OneDirBox *>(object_))
            one_dir_box_->rotate();
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
        selected_tile_position = grid_offset.Add(
          selected_grid_tile_index.Scale(current_tile_size)
        );
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
      // TODO
    }

    // Deselect button
    const Rectangle deselect_button = {
      (view_control_padding + view_control_space_inbetween)
        + view_control_button_width,
      current_window_height - 2 * view_control_button_height - 5,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(deselect_button, "Deselect"))
      is_currently_placing = false;

    // Switch button (?)
    const Rectangle switch_button = {
      (view_control_padding + 2 * view_control_space_inbetween)
        + (2 * view_control_button_width),
      current_window_height - 2 * view_control_button_height - 5,
      view_control_button_width,
      view_control_button_height
    };
    if (GuiButton(switch_button, "Switch")) {
      // TODO
    }

    // TILES
    // Selection outline
    if (is_currently_placing) {
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
    if (GuiButton(floor_tile, "")) {
      currently_selected_tile_type = TileType::Floor;
      switch_selection(floor_tile);
    }
    DrawRectangleRec(floor_tile, raylib::Color::Red());

    // Roof tile
    const Rectangle roof_tile = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(roof_tile, "")) {
      currently_selected_tile_type = TileType::Roof;
      switch_selection(roof_tile);
    }
    DrawRectangleRec(roof_tile, raylib::Color::Red());

    // Player tile
    const Rectangle player_tile = {
      tile_first_col_x,
      tile_picker_box_padding + tile_picker_delta,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(player_tile, "")) {
      currently_selected_object_type = ObjectType::Player;
      switch_selection(player_tile);
    }
    DrawRectangleRec(player_tile, raylib::Color::Red());

    // Goal tile
    const Rectangle goal_tile = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding + tile_picker_delta,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(goal_tile, "")) {
      currently_selected_tile_type = TileType::Goal;
      switch_selection(goal_tile);
    }
    DrawRectangleRec(goal_tile, raylib::Color::Red());

    // Button tile
    const Rectangle button_tile = {
      tile_first_col_x,
      tile_picker_box_padding + tile_picker_delta * 2,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(button_tile, "")) {
      currently_selected_tile_type = TileType::Button;
      switch_selection(button_tile);
    }
    DrawRectangleRec(button_tile, raylib::Color::Red());

    // Door tile
    const Rectangle door_tile = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding + tile_picker_delta * 2,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(door_tile, "")) {
      currently_selected_tile_type = TileType::Door;
      switch_selection(door_tile);
    }
    DrawRectangleRec(door_tile, raylib::Color::Red());

    // Portal tile
    const Rectangle portal_tile = {
      tile_first_col_x,
      tile_picker_box_padding + tile_picker_delta * 3,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(portal_tile, "")) {
      currently_selected_tile_type = TileType::Portal;
      switch_selection(portal_tile);
    }
    DrawRectangleRec(portal_tile, raylib::Color::Red());

    // Box tile
    const Rectangle box_tile = {
      tile_first_col_x + tile_picker_delta,
      tile_picker_box_padding + tile_picker_delta * 3,
      tile_picker_box_size,
      tile_picker_box_size
    };
    if (GuiButton(box_tile, "")) {
      currently_selected_object_type = ObjectType::Box;
      switch_selection(box_tile);
    }
    DrawRectangleRec(box_tile, raylib::Color::Red());

    window.EndDrawing();
  }
  return 0;
}
