enable_testing()

file(GLOB SOURCES_CXX "*.cc")
set(SOURCES ${SOURCES_CXX})

add_executable(tests)
target_sources(tests PRIVATE ${SOURCES})
target_link_libraries(tests PRIVATE
  common
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
