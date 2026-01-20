# Modern C++ Practices

A C++ project using Google Test for unit testing, managed with vcpkg.

## Requirements

- C++23 compatible compiler (project configured for C++23)
- CMake 3.16 or higher
- vcpkg package manager

## C++ Standard Support

This project is configured for C++23. To use C++26 features when available:

1. Ensure your compiler supports C++26 (e.g., MSVC with `/std:c++latest`)
2. Update `CMAKE_CXX_STANDARD` to 26 in `CMakeLists.txt`
3. Rebuild the project

## Available Samples

The project includes examples of various Modern C++ practices:

1. **01_RAII** - Resource Acquisition Is Initialization
2. **02_SFINAE** - Substitution Failure Is Not An Error
3. **03_CRTP** - Curiously Recurring Template Pattern
4. **04_PIMPL** - Pointer to Implementation
5. **05_RuleOfFive** - Rule of Five (constructors and assignment operators)
6. **06_TypeErasure** - Type Erasure
7. **07_VariantVisitor** - Variant and Visitor pattern
8. **08_SmartPointers** - Smart Pointers usage
9. **09_ExceptionSafety** - Exception Safety guarantees
10. **10_MoveSemantics** - Move Semantics

Each sample demonstrates the concept with real-world examples and includes unit tests.

## Building

1. Install dependencies: `vcpkg install`

2. Configure: `cmake -B build -S .`

3. Build: `cmake --build build`

4. Run tests: `ctest --test-dir build`

5. Run main: `./build/main`

## Running Samples

- List samples: `./build/main`, `bash run.sh`, or `run.bat`
- Run specific sample by number: `./build/main <number>`, `bash run.sh <number>`, or `run.bat <number>`
- Run by name (bash only): `bash run.sh <SampleName>` (e.g., `bash run.sh RAII`)