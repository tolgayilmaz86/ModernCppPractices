# Modern C++ Practices

![Build Status](https://github.com/tolgayilmaz86/ModernCppPractices/actions/workflows/build.yml/badge.svg)

A comprehensive collection of Modern C++ practice examples demonstrating key idioms, patterns, and techniques. Each example is runnable, tested, and includes detailed documentation.

## 🏗️ Project Structure

```
ModernCppPractices/
├── src/                         # Source code directory
│   ├── Testable.hpp             # Abstract base class for samples
│   ├── main.cpp                 # Main application entry point
│   └── XX_SampleName/           # Resource Acquisition Is Initialization
│       ├── README.md            # Detailed explanation
│       ├── XXXXSample.hpp       # Sample class header
│       └── XXXXSample.cpp       # Sample implementation
.
.
.
├── tests/                       # Unit testing framework
│   ├── CMakeLists.txt          # Test build configuration
│   └── test.cpp                # Test suite implementation
├── run.sh                      # Bash runner script (Linux/macOS)
├── run.bat                     # Windows batch runner script
├── CMakeLists.txt              # Main build configuration
├── vcpkg.json                  # Package manifest
├── .gitignore                  # Git ignore rules
└── README.md                   # This file
```

## 🔧 How It Works

### Architecture Overview

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   User Input    │───▶│  Sample Runner   │───▶│  Testable Impl │
│  (Command Line) │    │   (main.cpp)     │    │  (01_RAII, etc) │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │
                                ▼
                       ┌──────────────────┐
                       │   Test Suite     │
                       │  (Google Test)   │
                       └──────────────────┘
```

### Core Components

1. **Testable Interface** (`Testable.hpp`)
   - Abstract base class defining the contract for all samples
   - `run()`: Executes the sample demonstration
   - `name()`: Returns human-readable sample name

2. **Sample System**
   - Each practice has its own numbered directory (01_, 02_, etc.)
   - Each sample inherits from `Testable`
   - Consistent interface allows uniform execution and testing

3. **Runner Scripts**
   - `run.sh` / `run.bat`: Cross-platform sample execution
   - Support both numeric (1, 01) and named (RAII, SFINAE) inputs
   - Automatic build checking and executable detection

### Test Integration

```
Test Execution Flow:
1. CMake discovers all sample implementations
2. Google Test framework loads test cases
3. Each sample's run() method is called in test context
4. Output validation ensures samples execute correctly
5. Test results reported with detailed diagnostics
```

### Quick Reference: When to Use Each Idiom

| Idiom | Use When |
|-------|----------|
| **RAII** | Managing ANY resource (memory, files, locks, connections) |
| **SFINAE** | Pre-C++20 template constraints (prefer Concepts in C++20) |
| **CRTP** | Static polymorphism, mixins, avoiding virtual overhead |
| **Pimpl** | Reducing compile times, ABI stability, hiding dependencies |
| **Copy-and-Swap** | Exception-safe assignment operators |
| **Type Erasure** | Storing unrelated types uniformly, runtime flexibility |
| **Tag Dispatch** | Compile-time algorithm selection based on type traits |
| **Variant + Visitor** | Type-safe unions, fixed set of types, avoiding inheritance |
| **Smart Pointers** | Automatic memory management, ownership semantics |
| **Exception Safety** | Writing robust code with strong exception guarantees |
| **Move Semantics** | Efficient resource transfer, avoiding unnecessary copies |
| **Tag Dispatching** | Compile-time algorithm selection based on type traits |
| **Deep vs Shallow Copy** | Understanding copy semantics for complex objects |
| **Copy and Swap Idiom** | Exception-safe assignment operators |
| **C++ Cast Types** | Safe type conversions, avoiding C-style casts |
| **Thread Safety** | Writing concurrent code, avoiding data |
| **Concepts** | Template constraints, clearer intent in generic code |
| **Template Metaprogramming** | Compile-time computation, type traits, and static assertions |
| **Constexpr Programming** | Compile-time evaluation, performance optimization |
| **Lambda Expressions** | Inline function objects, closures, and functional programming |
| **Range-based Algorithms** | Cleaner iteration, STL algorithms, and functional style |
| **Structured Bindings** | Decomposing tuples, pairs, and structured data |

## 🚀 Building and Running

### Prerequisites

- **C++23 compatible compiler** (MSVC 19.3+, GCC 12+, Clang 15+)
- **CMake 3.16+**
- **vcpkg package manager**

### Quick Start

```bash
# 1. Clone the repository
git clone <repository-url>
cd ModernCppPractices

# 2. Install dependencies
vcpkg install

# 3. Configure and build
cmake -B build -S .
cmake --build build

# 4. Run tests
ctest --test-dir build

# 5. Explore samples
./build/main                    # List all samples
./build/main 1                  # Run RAII sample
bash run.sh SFINAE              # Run SFINAE by name
```

### Detailed Build Process

#### Dependency Installation
```bash
vcpkg install
```
This installs Google Test and other required packages locally.

#### CMake Configuration
```bash
cmake -B build -S .
```
- Configures the build system
- Detects compiler capabilities
- Sets up include paths and library linking

#### Compilation
```bash
cmake --build build
```
- Compiles all source files
- Links against Google Test
- Generates executable and test binaries

#### Test Execution
```bash
ctest --test-dir build
```
- Runs all unit tests
- Each sample is executed in test context
- Validates sample functionality

### Running Individual Samples

#### Using the Main Executable
```bash
# List available samples
./build/main

# Run by number
./build/main 1    # RAII
./build/main 2    # SFINAE
...

# Available samples:
# 1: Resource Acquisition Is Initialization
# 2: SFINAE - Substitution Failure Is Not An Error
# 3: CRTP - Curiously Recurring Template Pattern
# 4: PIMPL - Pointer to Implementation
# 5: Rule of Five
# 6: Type Erasure
# 7: Variant and Visitor Pattern
# 8: Smart Pointers
# 9: Exception Safety
# 10: Move Semantics
# 11: Tag Dispatching
# 12: Deep vs Shallow Copy
# 13: Copy and Swap Idiom
# 14: C++ Cast Types
# 15: Thread Safety
# 16: Concepts
# 17: Coroutines
# 18: Single Responsibility Principle
# 19: Open/Closed Principle
# 20: Liskov Substitution Principle
# 21: Interface Segregation Principle
# 22: Dependency Inversion Principle
# 23: UML Relationships
# 24: Three Way Comparison Operator
# 25: Projections
# 26: Input/Output Streams 
# 27: RunTime Type Information
# 28: Template Metaprogramming
# 29: Inplace Factory
```

#### Using Runner Scripts
```bash
# Cross-platform scripts with enhanced features
bash run.sh 1              # Run by number
bash run.sh RAII           # Run by name
run.bat 02                 # Windows batch script
```

### Run/Debug Configurations

The project includes several debug configurations in `.vscode/launch.json`:

| Configuration | Purpose | When to Use |
|---------------|---------|-------------|
| **Run Single Sample (Debug)** | Debug one specific sample | Learning a specific C++ concept |
| **Run All Samples (Debug)** | Debug through all samples sequentially | Understanding sample flow or testing breakpoints |
| **Run Unit Tests (Debug)** | Debug the test suite | Investigating test failures |
| **Debug Tests** | Debug individual test cases | Deep testing analysis |

### Development Workflow

#### Adding a New Sample

This project uses an **auto-registration system** that makes adding new samples simple. Follow these steps:

1. **Create Directory Structure**
   ```bash
   mkdir src/17_NewTopic
   ```

2. **Implement Sample Header**
   ```cpp
   // src/17_NewTopic/NewTopicSample.hpp
   #pragma once
   #include "Testable.hpp"

   class NewTopicSample : public Testable {
   public:
       void run() override;
       std::string name() const override {
           return "17_NewTopic - Your Topic Description";
       }
   };
   ```

3. **Add Implementation with Auto-Registration**
   ```cpp
   // src/17_NewTopic/NewTopicSample.cpp
   #include "NewTopicSample.hpp"
   #include <iostream>

   void NewTopicSample::run() {
       std::cout << "Running New Topic Sample..." << std::endl;
       // Your demonstration code here
   }

   // Auto-register this sample (add at end of file)
   #include "SampleRegistry.hpp"
   REGISTER_SAMPLE(NewTopicSample, "New Topic", 17);
   ```

4. **Include Header in main.cpp** (for linking)
   ```cpp
   // src/main.cpp - add with other includes
   #include "17_NewTopic/NewTopicSample.hpp"
   ```

5. **Add Tests**
   ```cpp
   // tests/test.cpp
   #include "17_NewTopic/NewTopicSample.hpp"
   TEST(Samples, NewTopic) {
       NewTopicSample sample;
       sample.run();
   }
   ```

6. **Update tests/CMakeLists.txt**
   ```cmake
   # Add the new .cpp file to the unit_tests executable
   add_executable(unit_tests test.cpp
       ...existing files...
       ../src/17_NewTopic/NewTopicSample.cpp)
   ```

7. **Create Documentation**
   - Add `src/17_NewTopic/README.md` with detailed explanation

**That's it!** The `REGISTER_SAMPLE` macro automatically registers your sample with the runner system. The number (17) controls the display order.

## 📋 Requirements Summary

| Component | Version | Purpose |
|-----------|---------|---------|
| C++ Compiler | C++23 | Language standard |
| CMake | 3.16+ | Build system |
| vcpkg | Latest | Package management |
| Google Test | 1.17+ | Testing framework |

## 🔄 Continuous Integration

The project is designed to work with CI/CD pipelines:

```yaml
# Example GitHub Actions workflow
- name: Install dependencies
  run: vcpkg install

- name: Configure
  run: cmake -B build -S .

- name: Build
  run: cmake --build build

- name: Test
  run: ctest --test-dir build
```

## 🤝 Contributing

1. Follow the established directory structure
2. Implement the `Testable` interface
3. Add comprehensive tests
4. Update documentation
5. Ensure cross-platform compatibility

This architecture ensures the project remains maintainable, testable, and educational for Modern C++ practices!