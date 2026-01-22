cmake_minimum_required(VERSION 3.16)

# Set up vcpkg using VCPKG_ROOT environment variable (must be before project())
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    else()
        message(WARNING "VCPKG_ROOT environment variable not set. Please set it to your vcpkg installation path.")
    endif()
endif()

project(ModernCppPractices LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include_directories(src)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(main ${SOURCES})

# Enable strict compile warnings
target_compile_options(main PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wconversion -Wshadow>
)

# Add tests subdirectory (GTest is found there)
add_subdirectory(tests)

enable_testing()