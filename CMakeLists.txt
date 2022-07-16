cmake_minimum_required(VERSION 3.13)
project(visual-sort CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(VISUAL_SORT_IS_ROOT_PROJECT ON)
else()
    set(VISUAL_SORT_IS_ROOT_PROJECT OFF)
endif()

option(VISUAL_SORT_TEST_WITH_SANITIZERS
       "Add address and undefined behavior sanitizers when building tests" ON
)

add_subdirectory(src)
add_subdirectory(tests)
