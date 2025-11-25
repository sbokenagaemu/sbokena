include(FetchContent)

if (DEFINED ENV{GOOGLETEST_SRC})
  FetchContent_Declare(
    googletest
    SOURCE_DIR $ENV{GOOGLETEST_SRC}
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

if (DEFINED ENV{NFD_SRC})
  FetchContent_Declare(
    nfd
    SOURCE_DIR $ENV{NFD_SRC}
    SYSTEM
  )
else ()
  FetchContent_Declare(
    nfd
    GIT_REPOSITORY "https://github.com/btzy/nativefiledialog-extended"
    GIT_TAG v1.2.1
    SYSTEM
  )
endif ()

if (DEFINED ENV{NLOHMANN_JSON_SRC})
  FetchContent_Declare(
    nlohmann_json
    SOURCE_DIR $ENV{NLOHMANN_JSON_SRC}
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

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if (NOT DEFINED GLFW_BUILD_WAYLAND)
    set(GLFW_BUILD_WAYLAND 1)
  endif ()
  if (NOT DEFINED GLFW_BUILD_X11)
    set(GLFW_BUILD_X11 1)
  endif ()
endif ()

if (DEFINED ENV{RAYLIB_SRC})
  FetchContent_Declare(
    raylib
    SOURCE_DIR $ENV{RAYLIB_SRC}
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

if (DEFINED ENV{RAYLIB_CPP_SRC})
  FetchContent_Declare(
    raylib_cpp
    SOURCE_DIR $ENV{RAYLIB_CPP_SRC}
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

if (DEFINED ENV{RAYGUI_SRC})
  FetchContent_Declare(
    raygui
    SOURCE_DIR $ENV{RAYGUI_SRC}
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
  nfd
  nlohmann_json
  raylib
  raylib_cpp
  raygui
)
