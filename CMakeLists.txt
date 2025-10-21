cmake_minimum_required(VERSION 4.0)
project(
  sbokena
  VERSION 0.1.0
  HOMEPAGE_URL https://github.com/sbokenagaemu/sbokena
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_link_options(-fuse-ld=lld)

# raylib wayland needs this
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
add_subdirectory(vendor)

# only add pedantic diagnostics for our own code
add_compile_options(-Wall -Wextra -Wpedantic)

# internal modules
add_subdirectory(common)
add_subdirectory(tests)
