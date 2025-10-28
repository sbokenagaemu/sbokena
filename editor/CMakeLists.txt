include(FetchContent)
file(GLOB SOURCES_CXX "src/*.cc")
file(GLOB SOURCES_HXX "include/*.hh")
set(SOURCES ${SOURCES_CXX} ${SOURCES_HXX})

add_executable(editor)
target_sources(editor PRIVATE ${SOURCES})
target_include_directories(editor PRIVATE include)
target_include_directories(editor PRIVATE
  "${FETCHCONTENT_BASE_DIR}/raygui-src/src"
  include
)
target_link_libraries(editor PRIVATE raylib raylib_cpp)
target_link_libraries(editor PRIVATE common)