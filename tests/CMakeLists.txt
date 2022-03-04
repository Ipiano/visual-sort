find_package(GTest REQUIRED)

add_executable(sort_tests sort_tests.cpp)
target_link_libraries(sort_tests PRIVATE GTest::gtest_main GTest::gmock)
