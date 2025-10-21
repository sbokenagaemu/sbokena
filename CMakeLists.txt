cmake_minimum_required(VERSION 4.0)
project(
  sbokena
  VERSION 0.1.0
  HOMEPAGE_URL https://github.com/sbokenagaemu/sbokena
  LANGUAGES CXX
)

# C++ standard & editor support
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# enable LTO/IPO in release builds
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if (ipo_supported)
  message("Building with LTO/IPO...")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
endif ()

# enable sanitizers in debug
if (CMAKE_BUILD_TYPE STREQUAL Debug)
  message("Building with sanitizers...")
  add_compile_options(-fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif ()

add_link_options(-fuse-ld=lld)

# raylib LTO/IPO needs this
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
# raylib wayland needs this
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
add_subdirectory(vendor)

# only add pedantic diagnostics for our own code
add_compile_options(-Wall -Wextra -Wpedantic)

# internal modules
add_subdirectory(common)
add_subdirectory(tests)
