# cmake/clang_tidy.cmake
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found")
  return()
endif()

message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")

# Collect sources to analyze (adapt as your project grows)
set(CLANG_TIDY_SOURCES
    src/moderncli_version.cpp
    tests/test_basic.cpp
)

add_custom_target(
  clang-tidy-check
  COMMAND ${CLANG_TIDY_EXE}
          -p ${CMAKE_BINARY_DIR}
          ${CLANG_TIDY_SOURCES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running clang-tidy on selected sources"
  VERBATIM
)