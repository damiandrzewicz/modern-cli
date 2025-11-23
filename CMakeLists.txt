cmake_minimum_required(VERSION 3.25)

project(
    moderncli
    VERSION 0.1.0
    DESCRIPTION "Modern, type-safe, reflection-based CLI framework for C++23"
    LANGUAGES CXX
)

# Export compile_commands.json for tooling (clangd, linters, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(MODERNCLI_BUILD_TESTS "Build unit tests" ON)
option(MODERNCLI_BUILD_EXAMPLES "Build examples" ON)
option(MODERNCLI_ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)

# Global C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
endif()

# Add library (header-only: INTERFACE)
add_library(moderncli INTERFACE)
add_library(moderncli::moderncli ALIAS moderncli)

target_include_directories(
    moderncli
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Public compile features
target_compile_features(moderncli INTERFACE cxx_std_23)

# Optional: general warning flags for consumers when building in-tree
if (MSVC)
    target_compile_options(moderncli INTERFACE /W4 /permissive- /EHsc)
else()
    target_compile_options(moderncli INTERFACE -Wall -Wextra -Wpedantic -Wconversion)
endif()

# Version source (non-header-only bit, for tools/tests)
add_library(moderncli_version STATIC src/moderncli_version.cpp)
target_link_libraries(moderncli_version PRIVATE moderncli)

# Clang-tidy (opt-in)
if (MODERNCLI_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if (CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy requested but not found")
    endif()
endif()

# Tests
# include(CTest)
if (MODERNCLI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if (MODERNCLI_BUILD_EXAMPLES)
    # add_subdirectory(examples)
endif()

# Install rules (for later packaging)
include(GNUInstallDirs)

install(
    TARGETS moderncli moderncli_version
    EXPORT moderncliTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT moderncliTargets
    NAMESPACE moderncli::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/moderncli
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/moderncliConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/moderncliConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/moderncliConfig.cmake"
    @ONLY
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/moderncliConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/moderncliConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/moderncli
)

include(cmake/clang_tidy.cmake)