// A file containing implementations regarding importing a loaded
// level into the editor (as an editor level) or exporting a loaded
// level (as a game level).

#pragma once

#include <nlohmann/json.hpp>

#include "editor_level.hh"
#include "object.hh"
#include "tile.hh"
// TODO: include common/loaded_level.hh

using nlohmann::json;
using namespace sbokena::editor::level;

// TODO: using namespace loaded_level

namespace Editor = sbokena::editor;

namespace sbokena::editor::from_to_json {

//  ===== objects =====

void from_json();
void to_json(Editor::object::Object &object);

// ===== tiles =====

void from_json();
void to_json(Editor::tile::Tile &tile);

// ===== level =====

void from_json();
void to_from(Editor::level::Level &level);

} // namespace sbokena::editor::from_to_json
