file(GLOB SOURCES_CXX "src/*.cc")
file(GLOB SOURCES_HXX "include/*.hh")
set(SOURCES ${SOURCES_CXX} ${SOURCES_HXX})

set(SOURCES_CXX_LIB ${SOURCES_CXX})
list(REMOVE_ITEM SOURCES_CXX_LIB "src/main.cc")
set(SOURCES_LIB ${SOURCES_CXX_LIB} ${SOURCES_HXX})

add_executable(editor)
target_sources(editor PRIVATE ${SOURCES})
target_include_directories(editor PRIVATE include)
target_link_libraries(editor
  PRIVATE
    raylib
    raylib_cpp
    raygui
    common
)

add_library(libeditor)
target_sources(libeditor PRIVATE ${SOURCES_LIB})
target_include_directories(libeditor PUBLIC include)
target_link_libraries(libeditor
  PUBLIC
    raylib
    raylib_cpp
    raygui
    common
)