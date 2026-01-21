cmake_minimum_required(VERSION 3.16)

project(ModernCppPractices LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Set up vcpkg
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows" CACHE STRING "")
endif()

include_directories(src)

# GTest is only needed for tests
# find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(main ${SOURCES})

# Main executable doesn't need GTest - only tests do
# target_link_libraries(main GTest::gtest_main)

# Temporarily disable tests until GTest cmake config is resolved
# add_subdirectory(tests)

enable_testing()