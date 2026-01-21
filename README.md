# Modern C++ Practices

A comprehensive collection of Modern C++ practice examples demonstrating key idioms, patterns, and techniques. Each example is runnable, tested, and includes detailed documentation.

## 🏗️ Project Structure

```
ModernCppPractices/
├── src/                          # Source code directory
│   ├── Testable.hpp             # Abstract base class for samples
│   ├── main.cpp                 # Main application entry point
│   └── 01_RAII/                 # Resource Acquisition Is Initialization
│       ├── README.md            # Detailed explanation
│       ├── RAIISample.hpp       # Sample class header
│       └── RAIISample.cpp       # Sample implementation
│   └── 02_SFINAE/               # Substitution Failure Is Not An Error
│       ├── README.md
│       ├── SFINAESample.hpp
│       └── SFINAESample.cpp
│   └── 03_CRTP/                 # Curiously Recurring Template Pattern
│       ├── README.md
│       ├── CRTPSample.hpp
│       └── CRTPSample.cpp
│   └── 04_PIMPL/                 # Pointer to Implementation
│       ├── README.md
│       ├── PimplSample.hpp
│       └── PimplSample.cpp
│   └── 05_RuleOfFive/            # Rule of Five
│       ├── README.md
│       ├── RuleOfFiveSample.hpp
│       └── RuleOfFiveSample.cpp
│   └── 06_TypeErasure/          # Type Erasure
│       ├── README.md
│       ├── TypeErasureSample.hpp
│       └── TypeErasureSample.cpp
│   └── 07_VariantVisitor/       # Variant and Visitor Pattern
│       ├── README.md
│       ├── VariantVisitorSample.hpp
│       └── VariantVisitorSample.cpp
│   └── 08_SmartPointers/         # Smart Pointers
│       ├── README.md
│       ├── SmartPointersSample.hpp
│       └── SmartPointersSample.cpp
│   └── [09-10]_*/               # Additional practice examples
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
```

#### Using Runner Scripts
```bash
# Cross-platform scripts with enhanced features
bash run.sh 1              # Run by number
bash run.sh RAII           # Run by name
run.bat 02                 # Windows batch script
```

### Development Workflow

#### Adding a New Sample

1. **Create Directory Structure**
   ```bash
   mkdir src/03_CRTP
   ```

2. **Implement Sample Class**
   ```cpp
   // src/03_CRTP/CRTPSample.hpp
   #pragma once
   #include "Testable.hpp"

   class CRTPSample : public Testable {
   public:
       void run() override;
       std::string name() const override {
           return "03_CRTP - Curiously Recurring Template Pattern";
       }
   };
   ```

3. **Add Implementation**
   ```cpp
   // src/03_CRTP/CRTPSample.cpp
   #include "CRTPSample.hpp"
   #include <iostream>

   void CRTPSample::run() {
       std::cout << "CRTP demonstration..." << std::endl;
   }
   ```

4. **Update Main Application**
   ```cpp
   // src/main.cpp
   #include "03_CRTP/CRTPSample.hpp"
   // Add to samples vector
   samples.push_back(std::make_unique<CRTPSample>());
   ```

5. **Add Tests**
   ```cpp
   // tests/test.cpp
   #include "03_CRTP/CRTPSample.hpp"
   TEST(Samples, CRTP) {
       CRTPSample sample;
       sample.run();
   }
   ```

6. **Update Build System**
   ```cmake
   # tests/CMakeLists.txt
   add_executable(unit_tests test.cpp
       ../src/01_RAII/RAIISample.cpp
       ../src/02_SFINAE/SFINAESample.cpp
       ../src/03_CRTP/CRTPSample.cpp
       ../src/04_PIMPL/PimplSample.cpp
       ../src/05_RuleOfFive/RuleOfFiveSample.cpp
       ../src/06_TypeErasure/TypeErasureSample.cpp
       ../src/07_VariantVisitor/VariantVisitorSample.cpp
       ../src/08_SmartPointers/SmartPointersSample.cpp)  # Add new sample
   ```

7. **Update Documentation**
   - Add to main README.md
   - Create detailed README.md in sample directory
   - Update runner scripts if needed

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