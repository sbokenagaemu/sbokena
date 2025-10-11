cmake_minimum_required(VERSION 4.0)
project(sbokena
        VERSION 0.1.0
        HOMEPAGE_URL https://github.com/sbokenagaemu/sbokena
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES_CXX "src/*.cc")
file(GLOB SOURCES_HXX "include/*.hh")
set(SOURCES ${SOURCES_CXX} ${SOURCES_HXX})

add_link_options("-fuse-ld=lld")
find_package(raylib 5.5 REQUIRED)

add_executable(main)
target_sources(main PRIVATE ${SOURCES})
target_include_directories(main PRIVATE include)
target_compile_options(main PRIVATE
  -Wall -Wextra -Wpedantic
)
target_link_libraries(main PRIVATE raylib)
