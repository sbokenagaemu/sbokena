cmake_minimum_required(VERSION 4.0)
project(
  sbokena
  VERSION 0.1.0
  HOMEPAGE_URL https://github.com/sbokenagaemu/sbokena
  LANGUAGES CXX
)
if (APPLE)
  # macOS only
  # Request OpenGL 4.1 Core Profile (macOS maximum)
  add_compile_definitions(MACOS_COCOA_BACKEND)
  set(GLFW_USE_COCOA TRUE)
  set(GLFW_USE_METAL FALSE)  # optional, only if using OpenGL, not Metal
endif()

# disable dev warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)

# C++ standard & editor support
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# enable LLD for all targets
add_link_options(-fuse-ld=lld)

# Only Linux should include Wayland/X11
if(UNIX AND NOT APPLE)
  set(ENABLE_WAYLAND TRUE)
  set(ENABLE_X11 TRUE)
endif()

if (APPLE)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(OpenGL_LIBRARY OpenGL REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
endif()

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

if (APPLE)
  target_link_libraries(game PRIVATE ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

# enable asan and ubsan for tests
add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
add_subdirectory(tests)
