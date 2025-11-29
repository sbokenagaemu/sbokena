file(GLOB SOURCES_CXX "src/*.cc")
file(GLOB SOURCES_HXX "include/*.hh")
set(SOURCES ${SOURCES_CXX} ${SOURCES_HXX})

set(SOURCES_CXX_LIB ${SOURCES_CXX})
list(REMOVE_ITEM SOURCES_CXX_LIB "src/main.cc")
set(SOURCES_LIB ${SOURCES_CXX_LIB} ${SOURCES_HXX})

add_executable(game)
target_sources(game PRIVATE ${SOURCES})
target_include_directories(game PRIVATE include)
target_link_libraries(game
  PRIVATE
    nfd
    raylib
    raygui
    common
)

add_library(libgame)
target_sources(libgame PRIVATE ${SOURCES_LIB})
target_include_directories(libgame PUBLIC include)
target_link_libraries(libgame
  PUBLIC
    nfd
    raylib
    raygui
    common
)
