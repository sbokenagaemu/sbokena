include(FetchContent)

set(GLFW_BUILD_WAYLAND ON)
set(GLFW_BUILD_X11 ON)

if (DEFINED ENV{raylib_src})
  FetchContent_Declare(
    raylib
    SOURCE_DIR "$ENV{raylib_src}"
  )
else ()
  FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib"
    GIT_TAG 5.5
  )
endif ()

if (DEFINED ENV{raylib_cpp_src})
  FetchContent_Declare(
    raylib_cpp
    SOURCE_DIR "$ENV{raylib_cpp_src}"
  )
else ()
  FetchContent_Declare(
    raylib_cpp
    GIT_REPOSITORY "https://github.com/RobLoach/raylib-cpp"
    GIT_TAG v5.5.0
  )
endif ()

FetchContent_MakeAvailable(raylib)
FetchContent_MakeAvailable(raylib_cpp)
