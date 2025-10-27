include(FetchContent)

if (DEFINED ENV{googletest_src})
  FetchContent_Declare(
    googletest
    SOURCE_DIR "$ENV{googletest_src}"
  )
else ()
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest"
    GIT_TAG v1.17.0
  )
endif ()

set(GLFW_BUILD_WAYLAND ON)

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

if (DEFINED ENV{raygui_src})
  FetchContent_Declare(
    raygui
    SOURCE_DIR "$ENV{raygui_src}"
  )
  else ()
    FetchContent_Declare(
      raygui
      GIT_REPOSITORY "https://github.com/raysan5/raygui"
      GIT_TAG 4.0
    )
endif ()

FetchContent_MakeAvailable(
  googletest
  raylib
  raylib_cpp
  raygui
)
