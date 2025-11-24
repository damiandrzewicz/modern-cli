include(CTest)

option(MCLI_ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

if (MCLI_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    message(STATUS "Enabling coverage flags")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

if (MCLI_BUILD_TESTS)
    add_custom_target(
        coverage
        COMMAND ${CMAKE_CTEST_COMMAND} --preset dev-test --output-on-failure
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running tests for coverage (report generation can be added later)"
    )
endif()
