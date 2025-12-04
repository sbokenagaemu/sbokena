if (BUILD_TESTS)
  include(FetchContent)

  enable_testing()

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
  FetchContent_MakeAvailable(googletest)

  file(GLOB SOURCES_CXX "*.cc")
  set(SOURCES ${SOURCES_CXX})

  add_executable(tests)
  target_sources(tests PRIVATE ${SOURCES})
  target_link_libraries(tests PRIVATE
    GTest::gtest_main
    nlohmann_json::nlohmann_json
    common
    libgame
  )

  include(GoogleTest)
  gtest_discover_tests(tests)
endif ()
