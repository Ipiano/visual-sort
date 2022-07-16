find_package(GTest REQUIRED)

add_executable(sort_tests sort_tests.cpp)
target_link_libraries(sort_tests PRIVATE GTest::gtest_main GTest::gmock sort_algorithms)

if(VISUAL_SORT_TEST_WITH_SANITIZERS)
    include(Sanitizers)
    target_link_libraries(sort_tests PRIVATE fsanitize::undefined fsanitize::address)
endif()

set(SORT_TEST_OUTPUT ${VISUAL_SORT_REPORTS_DIR}/sort-tests.xml)
add_custom_command(
    OUTPUT "${SORT_TEST_OUTPUT}"
    COMMAND $<TARGET_FILE:sort_tests> --gtest_color=yes "--gtest_output=xml:${SORT_TEST_OUTPUT}"
    USES_TERMINAL
)

add_custom_target(run-sort-tests DEPENDS ${SORT_TEST_OUTPUT})

if(VISUAL_SORT_IS_ROOT_PROJECT)
    add_custom_target(check)
    add_dependencies(check run-sort-tests)
endif()

if(VISUAL_SORT_INSTRUMENT_COVERAGE)
    find_package(gcovr 4.2 REQUIRED)

    target_instrument_for_coverage(sort_tests PRIVATE)

    file(MAKE_DIRECTORY "${VISUAL_SORT_REPORTS_DIR}/sort-coverage")

    add_custom_target(
        sort-coverage
        COMMAND
            "${GCOVR_PATH}" --xml "${VISUAL_SORT_REPORTS_DIR}/sort-coverage.xml" --html-details
            "${VISUAL_SORT_REPORTS_DIR}/sort-coverage/sort-coverage.html" --gcov-executable
            "${GCOV_COMMAND}" --branches --exclude-unreachable-branches --exclude-throw-branches -s
            --filter "${PROJECT_SOURCE_DIR}/src/sort_algorithms"
        BYPRODUCTS "${VISUAL_SORT_REPORTS_DIR}/sort-coverage.xml"
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    )

    # If root project, create coverage target alias
    if(VISUAL_SORT_IS_ROOT_PROJECT)
        add_custom_target(coverage)
        add_dependencies(coverage sort-coverage)
    endif()
endif()
