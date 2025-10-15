file(GLOB SOURCES_CXX "src/*.cc")
file(GLOB SOURCES_HXX "include/*.hh")
set(SOURCES ${SOURCES_CXX} ${SOURCES_HXX})

add_library(common)
target_sources(common PRIVATE ${SOURCES})
target_include_directories(common PUBLIC include)
target_link_libraries(common PRIVATE raylib raylib_cpp)
