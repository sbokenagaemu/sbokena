include(FetchContent)

if (DEFINED GOOGLETEST_SRC)
  FetchContent_Declare(
    googletest
    SOURCE_DIR ${GOOGLETEST_SRC}
    SYSTEM
  )
else ()
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest"
    GIT_TAG v1.17.0
    SYSTEM
  )
endif ()

if (DEFINED NLOHMANN_JSON_SRC)
  FetchContent_Declare(
    nlohmann_json
    SOURCE_DIR ${NLOHMANN_JSON_SRC}
    SYSTEM
  )
else ()
  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY "https://github.com/nlohmann/json"
    GIT_TAG v3.12.0
    SYSTEM
  )
endif ()

if (NOT DEFINED GLFW_BUILD_WAYLAND)
  set(GLFW_BUILD_WAYLAND 1)
endif ()
if (NOT DEFINED GLFW_BUILD_X11)
  set(GLFW_BUILD_X11 1)
endif ()

if (DEFINED RAYLIB_SRC)
  FetchContent_Declare(
    raylib
    SOURCE_DIR ${RAYLIB_SRC}
    SYSTEM
  )
else ()
  FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib"
    GIT_TAG 5.5
    SYSTEM
  )
endif ()

if (DEFINED RAYLIB_CPP_SRC)
  FetchContent_Declare(
    raylib_cpp
    SOURCE_DIR ${RAYLIB_CPP_SRC}
    SYSTEM
  )
else ()
  FetchContent_Declare(
    raylib_cpp
    GIT_REPOSITORY "https://github.com/RobLoach/raylib-cpp"
    GIT_TAG v5.5.0
    SYSTEM
  )
endif ()

if (DEFINED RAYGUI_SRC)
  FetchContent_Declare(
    raygui
    SOURCE_DIR ${RAYGUI_SRC}
    SOURCE_SUBDIR projects/CMake
    SYSTEM
  )
else ()
  FetchContent_Declare(
    raygui
    GIT_REPOSITORY "https://github.com/raysan5/raygui"
    GIT_TAG 4.0
    SOURCE_SUBDIR projects/CMake
    SYSTEM
  )
endif ()

FetchContent_MakeAvailable(
  googletest
  nlohmann_json
  raylib
  raylib_cpp
  raygui
)
