include(FetchContent)
file(GLOB SOURCES_CXX "src/*.cc")
file(GLOB SOURCES_HXX "include/*.hh")
set(SOURCES ${SOURCES_CXX} ${SOURCES_HXX})

add_executable(editor)
target_sources(editor PRIVATE ${SOURCES})
target_include_directories(editor PRIVATE include)
target_link_libraries(editor
  PRIVATE
    nlohmann_json::nlohmann_json
    raylib
    raylib_cpp
    raygui
    common
)
