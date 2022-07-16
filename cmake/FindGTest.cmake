find_package(GTest NO_MODULE QUIET)

if(NOT TARGET GTest_FOUND)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../submodules/googletest googletest)

    add_library(GTest::gtest ALIAS gtest)
    add_library(GTest::gtest_main ALIAS gtest_main)
    add_library(GTest::gmock ALIAS gmock)
    add_library(GTest::gmock_main ALIAS gmock_main)

    set(GTest_FOUND TRUE)
endif()
