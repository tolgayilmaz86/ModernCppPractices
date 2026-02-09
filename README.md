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

## 🚀 Building and Running

### Prerequisites

- **C++23 compatible compiler** (MSVC 19.3+, GCC 12+, Clang 15+)
- **CMake 3.16+**
- **vcpkg package manager**
- **Git** (for cloning and .gitignore)

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
bash run.sh SFINAE             # Run SFINAE by name
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

# Output example:
# Available samples:
# 1: 01_RAII - Resource Acquisition Is Initialization
# 2: 02_SFINAE - Substitution Failure Is Not An Error
# 3: 03_CRTP - Curiously Recurring Template Pattern
# 4: 04_PIMPL - Pointer to Implementation
# 5: 05_RuleOfFive - Rule of Five
# 6: 06_TypeErasure - Type Erasure
# 7: 07_VariantVisitor - Variant and Visitor Pattern
# 8: 08_SmartPointers - Smart Pointers
# 9: 09_ExceptionSafety - Exception Safety
# 10: Move Semantics
# 11: Tag Dispatching
# 12: Deep vs Shallow Copy
# 13: Copy and Swap Idiom
# 14: C++ Cast Types
# 15: Thread Safety
```

#### Using Runner Scripts
```bash
# Cross-platform scripts with enhanced features
bash run.sh 1              # Run by number
bash run.sh RAII           # Run by name
run.bat 02                 # Windows batch script
```

## 🐛 Debugging Samples

This project includes comprehensive debugging support for VS Code with pre-configured launch configurations. Debug any sample to step through code, inspect variables, and understand Modern C++ concepts in action.

### Prerequisites for Debugging

- **VS Code** with C++ extension installed
- **Project built in Debug mode** (required for debugging symbols)
- **Breakpoints** set in sample code

### Quick Debug Start

1. **Build in Debug mode:**
   ```bash
   cmake --build build --config Debug
   ```

2. **Open any sample file** (e.g., `src/06_TypeErasure/TypeErasureSample.cpp`)

3. **Set breakpoints** by clicking in the left gutter next to line numbers

4. **Start debugging:**
   - Press `F5` or go to **Run → Start Debugging**
   - Select **"Run Single Sample (Debug)"** from the dropdown
   - Choose your sample from the picker (e.g., "6 - Type Erasure")

### Debug Configurations

The project includes several debug configurations in `.vscode/launch.json`:

| Configuration | Purpose | When to Use |
|---------------|---------|-------------|
| **Run Single Sample (Debug)** | Debug one specific sample | Learning a specific C++ concept |
| **Run All Samples (Debug)** | Debug through all samples sequentially | Understanding sample flow or testing breakpoints |
| **Run Unit Tests (Debug)** | Debug the test suite | Investigating test failures |
| **Debug Tests** | Debug individual test cases | Deep testing analysis |

### Step-by-Step Debugging Guide

#### 1. Setting Breakpoints
```cpp
// Example: Set breakpoint in TypeErasureSample.cpp
void TypeErasureSample::run() {
    std::cout << "Running Type Erasure Sample..." << std::endl;  // ← Click here for breakpoint
    
    // Your breakpoint will hit here when debugging
    std::vector<AnyShape> shapes;
    shapes.emplace_back(CircleShape{5.0});  // ← Or here to inspect object creation
}
```

#### 2. Debug Controls
- **F5**: Continue execution to next breakpoint
- **F10**: Step over (execute current line, don't enter functions)
- **F11**: Step into (enter function calls)
- **Shift+F11**: Step out (exit current function)
- **Shift+F5**: Stop debugging

#### 3. Inspecting Variables
- **Variables Panel**: View local, global, and static variables
- **Watch Panel**: Add custom expressions (e.g., `shapes.size()`, `shape.area()`)
- **Hover**: Mouse over variables in code to see values
- **Debug Console**: Evaluate expressions like `shapes[0].area()`

#### 4. Advanced Debugging Features

**Conditional Breakpoints:**
- Right-click breakpoint → Add condition (e.g., `i == 5`)
- Break only when specific conditions are met

**Logpoints:**
- Right-click breakpoint → "Log Message"
- Print debug info without stopping execution

**Call Stack Inspection:**
- View function call hierarchy
- Jump to different stack frames
- Understand program flow

### Debugging Different Sample Types

#### Template-Heavy Samples (CRTP, SFINAE)
```cpp
// Set breakpoint in template instantiation
template <typename T>
void process(T&& value) {
    std::cout << "Processing: " << value << std::endl;  // Break here
}

// The debugger will show template parameters
process(42);  // T = int
process("hello");  // T = const char*
```

#### Polymorphic Code (Type Erasure, Inheritance)
```cpp
// Break in virtual function calls
void drawShape(const Drawable& shape) {
    shape.draw();  // Break here to see dynamic dispatch
}

// Inspect the actual derived type at runtime
```

#### Exception Safety Samples
```cpp
// Break on exception throw/catch
try {
    riskyOperation();  // Break here
    throw std::runtime_error("Error!");  // Or here
} catch (const std::exception& e) {
    std::cout << "Caught: " << e.what() << std::endl;  // Break here
}
```

### Troubleshooting Common Issues

#### Breakpoint Not Hit
- **Ensure Debug build**: `cmake --build build --config Debug`
- **Check configuration**: Select correct debug config from dropdown
- **Rebuild after changes**: Clean rebuild if code was modified

#### Variables Not Showing Values
- **Debug symbols missing**: Rebuild in Debug configuration
- **Optimized out**: Some variables may be optimized away in complex code
- **Use Watch window**: Add variables explicitly to watch

#### Program Runs But Doesn't Stop
- **Wrong sample selected**: Verify sample number in debug config
- **Conditional breakpoint**: Check if conditions are met
- **Release vs Debug**: Ensure using Debug configuration

### Debug Tips for Learning C++

- **Step through STL containers**: Watch how `std::vector`, `std::unique_ptr` work internally
- **Observe object lifetimes**: Set breakpoints in constructors/destructors
- **Template instantiation**: See how templates generate code at compile time
- **Memory layout**: Inspect object sizes and layouts with debugger
- **Performance analysis**: Use debugger to understand optimization opportunities

### Integration with Development Workflow

Debugging is integrated into the development process:

```bash
# 1. Make code changes
# 2. Build debug version
cmake --build build --config Debug

# 3. Set breakpoints in new code
# 4. Debug specific sample
# 5. Verify behavior
# 6. Run tests to ensure no regressions
ctest --test-dir build
```

This debugging setup makes it easy to experiment with Modern C++ concepts while understanding exactly how they work under the hood!

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

### Build System Details

#### CMake Configuration
- **C++23 Standard**: Ensures modern language features
- **Static Runtime**: Consistent linking across platforms
- **Recursive Source Discovery**: Automatically finds all .cpp files
- **Include Path Management**: Proper header resolution

#### Test Framework
- **Google Test Integration**: Industry-standard testing
- **Sample Validation**: Each practice example is tested
- **Cross-platform Execution**: Works on Windows, Linux, macOS

#### Package Management
- **vcpkg Manifest**: Declarative dependency management
- **Local Installation**: No system-wide package pollution
- **Reproducible Builds**: Consistent environments

## 🧪 Testing Strategy

### Unit Tests
- Each sample has dedicated test cases
- Validates sample execution without errors
- Ensures examples remain functional during development

### Integration Tests
- Build system verification
- Cross-platform compatibility
- Dependency resolution testing

### Sample Validation
- Runtime behavior verification
- Output consistency checking
- Error handling validation

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