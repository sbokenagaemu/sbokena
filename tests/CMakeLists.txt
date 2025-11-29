enable_testing()

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
