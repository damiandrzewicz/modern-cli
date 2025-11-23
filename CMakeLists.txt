cmake_minimum_required(VERSION 3.25)

project(
    mcli
    VERSION 0.1.0
    DESCRIPTION "Modern, type-safe, reflection-based CLI framework for C++23"
    LANGUAGES CXX
)

# Export compile_commands.json for tooling (clangd, linters, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(MCLI_BUILD_TESTS "Build unit tests" ON)
option(MCLI_BUILD_EXAMPLES "Build examples" ON)
option(MCLI_ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)

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
add_library(mcli INTERFACE)
add_library(mcli::mcli ALIAS mcli)

target_include_directories(
    mcli
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Public compile features
target_compile_features(mcli INTERFACE cxx_std_23)

# Optional: general warning flags for consumers when building in-tree
if (MSVC)
    target_compile_options(mcli INTERFACE /W4 /permissive- /EHsc)
else()
    target_compile_options(mcli INTERFACE -Wall -Wextra -Wpedantic -Wconversion)
endif()

# Version source (non-header-only bit, for tools/tests)
add_library(mcli_version STATIC src/version.cpp)
target_link_libraries(mcli_version PRIVATE mcli)

# Clang-tidy (opt-in)
if (MCLI_ENABLE_CLANG_TIDY)
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
if (MCLI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if (MCLI_BUILD_EXAMPLES)
    # add_subdirectory(examples)
endif()

# Install rules (for later packaging)
include(GNUInstallDirs)

install(
    TARGETS mcli mcli_version
    EXPORT mcliTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT mcliTargets
    NAMESPACE mcli::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mcli
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mcliConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcliConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/mcliConfig.cmake"
    @ONLY
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/mcliConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/mcliConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mcli
)

include(cmake/clang_tidy.cmake)