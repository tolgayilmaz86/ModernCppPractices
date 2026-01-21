cmake_minimum_required(VERSION 3.16)

project(ModernCppPractices LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include_directories(src)

# GTest is only needed for tests
# find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(main ${SOURCES})

# Main executable doesn't need GTest - only tests do
# target_link_libraries(main GTest::gtest_main)

add_subdirectory(tests)

enable_testing()