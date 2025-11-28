cmake_minimum_required(VERSION 3.25)

# download CPM.cmake
set(CPM_DOWNLOAD_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake")
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  "${CPM_DOWNLOAD_LOCATION}"
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)

include("${CPM_DOWNLOAD_LOCATION}")

CPMAddPackage(
    NAME GTest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.17.0
    OPTIONS "INSTALL_GTEST OFF"
)

add_executable(mcli_tests
    test_version.cpp
    test_flags.cpp
)

target_link_libraries(mcli_tests
    PRIVATE
        mcli::mcli
        mcli_version
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mcli_tests)
