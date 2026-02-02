# Ubuntu 22.04+, Fedora 42+
cmake_minimum_required(VERSION 3.31)

project(
  sbokena
  VERSION 0.1.0
  HOMEPAGE_URL https://github.com/sbokenagaemu/sbokena
  LANGUAGES CXX
)

# disable dev warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)

# C++ standard & editor support
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# macOS version requirements
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # `std::to_chars` is in 13.3 and up
  set(CMAKE_OSX_DEPLOYMENT_TARGET 13.3)
endif ()

# set default build type
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif ()

# enable LTO/IPO in release builds
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if (ipo_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
endif ()

# raylib LTO/IPO needs this
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
# raylib wayland needs this
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
# external dependencies
add_subdirectory(vendor)

# only add pedantic diagnostics for our own code
add_compile_options(-Wall -Wextra -Wpedantic)

# internal modules
add_subdirectory(common)
add_subdirectory(editor)
add_subdirectory(game)

# enable asan and ubsan for tests
add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
add_subdirectory(tests)
