cmake_minimum_required(VERSION 3.16)

project(ModernCppPractices LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Set up vcpkg
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Use local vcpkg_installed directory
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows" CACHE STRING "")

include_directories(src)

# GTest is only needed for tests
find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(main ${SOURCES})

# Main executable doesn't need GTest - only tests do
# target_link_libraries(main GTest::gtest_main)

# Add tests subdirectory
add_subdirectory(tests)

enable_testing()