# find_package implementation to aid coverage analysis
#
# This file will do the following
#
# * Find the 'gcovr' executable matching the version rules. This executable will be stored in
#   GCOVR_PATH
# * Find the 'gcov' executable that matches the CMAKE_CXX_COMPILER. It is assumed that all targets
#   instrumented for coverage are C/C++ and use that compiler. The command to invoke this tool will
#   be stored in GCOV_COMMAND
# * Create the 'target_instrument_for_coverage' function (see below)

# Find gcovr and extract version
if(NOT GCOVR_PATH)
    find_program(GCOVR_PATH gcovr DOC "gcovr executable path")

    if(GCOVR_PATH)
        execute_process(COMMAND ${GCOVR_PATH} --version OUTPUT_VARIABLE GCOVR_VERSION_TEXT)
        string(
            REGEX MATCH
                  "gcovr ([0-9]+[.][0-9]+)"
                  _UNUSED
                  "${GCOVR_VERSION_TEXT}"
        )
        set(GCOVR_VERSION ${CMAKE_MATCH_1})
        message(DEBUG "gcovr ${GCOVR_VERSION} path: ${GCOVR_PATH}")
    endif()
endif()

# Figure out what version of gcov to use
if(NOT GCOV_PATH)
    string(
        REGEX MATCH
              "([0-9]+)[.]?.*"
              _UNUSED
              "${CMAKE_CXX_COMPILER_VERSION}"
    )
    set(COMPILER_VERSION_MAJOR ${CMAKE_MATCH_1})

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(GCOV_BASE "llvm-cov")
        set(GCOV_ARGS "gcov")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(GCOV_BASE "gcov")
    endif()

    set(GCOV_NAME "${GCOV_BASE}-${COMPILER_VERSION_MAJOR}")
    find_program(GCOV_PATH "${GCOV_NAME}" DOC "gcov executable path")

    if(GCOV_PATH)
        set(GCOV_COMMAND
            "${GCOV_PATH} ${GCOV_ARGS}"
            CACHE STRING "Command to use when running gcov"
        )
        mark_as_advanced(GCOV_COMMAND)
    endif()

    message(DEBUG "${GCOV_NAME} command: ${GCOV_COMMAND}")
endif()

# instrument_coverage(TARGET LEVEL)
#
# Sets compilation options on TARGET appropriately to generate gcov files when building and running
# it.
function(target_instrument_for_coverage TARGET LEVEL)

    # NDEBUG so that assert() statements are removed
    target_compile_definitions(${TARGET} ${LEVEL} -DNDEBUG)

    # Clang and GCC both respect --coverage as a compile and link option
    target_compile_options(${TARGET} ${LEVEL} --coverage)
    target_link_options(${TARGET} ${LEVEL} --coverage)
endfunction()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    gcovr
    VERSION_VAR GCOVR_VERSION
    REQUIRED_VARS GCOVR_PATH GCOV_COMMAND
)
