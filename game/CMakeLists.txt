file(GLOB SOURCES_CXX "src/*.cc")
file(GLOB SOURCES_HXX "include/*.hh")
set(SOURCES ${SOURCES_CXX} ${SOURCES_HXX})

add_executable(game)
target_sources(game PRIVATE ${SOURCES})
target_include_directories(game PRIVATE include)
target_link_libraries(game
  PRIVATE
    nlohmann_json::nlohmann_json
    raylib
    raylib_cpp
    common
)
