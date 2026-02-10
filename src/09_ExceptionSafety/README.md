# Exception Safety

## Overview

Exception safety is a fundamental concept in Modern C++ that ensures operations either complete successfully or leave the program in a consistent, valid state when exceptions occur. This sample demonstrates the three levels of exception safety guarantees, RAII for automatic resource cleanup, exception specifications, and best practices for writing exception-safe code.

**The Problem:** Operations that throw exceptions mid-execution can leave objects in invalid states, cause resource leaks, or corrupt data structures.

**The Solution: Exception Safety Guarantees** Design operations to provide predictable behavior when exceptions occur, using RAII and careful state management.

## How Exception Safety Works

1. **Exception Safety Guarantees:** Three levels of guarantees for operation behavior during exceptions
2. **RAII Principle:** Automatic resource cleanup through destructors
3. **Transaction Pattern:** All-or-nothing operations with rollback capabilities
4. **noexcept Specifications:** Documenting which functions never throw
5. **Exception Handling:** Proper catching and handling of exceptions

```cpp
// Unsafe operation - may leave object in invalid state
void unsafe_operation() {
    data_.push_back(item);  // If this succeeds...
    process_item(item);     // ...but this throws, data_ is corrupted
}

// Exception-safe operation - strong guarantee
void safe_operation() {
    auto temp_data = data_;           // Copy current state
    temp_data.push_back(item);        // Modify copy
    process_item(item);               // If this throws, original unchanged
    data_ = std::move(temp_data);     // Commit changes
}
```

## Key Components

### 1. Exception Safety Guarantees
- **No Exception Safety:** Operations may leave objects in invalid states
- **Basic Guarantee:** Object invariants are preserved, but state may change
- **Strong Guarantee:** Operations either succeed completely or fail completely (rollback)
- **No-Throw Guarantee:** Operations never throw exceptions

### 2. RAII (Resource Acquisition Is Initialization)
- **Automatic Cleanup:** Resources are cleaned up in destructors and going out of scope
- **Exception Safety:** Resources are properly released even when exceptions occur
- **Stack Unwinding:** Destructors are called during exception propagation

### 3. Transaction Pattern
- **All-or-Nothing:** Complex operations either succeed completely or fail completely
- **Rollback Actions:** Registered cleanup functions for failure scenarios
- **State Preservation:** Original state maintained until operation commits

### 4. Exception Specifications
- **noexcept:** Functions that guarantee they won't throw
- **Conditional noexcept:** noexcept based on conditions
- **Documentation:** Clear indication of exception behavior

## Implementation Details

This sample demonstrates:

1. **Exception Safety Levels:** Testing the three levels of exception safety guarantees
2. **RAII Exception Safety:** Automatic resource cleanup with RAII objects
3. **Transaction Pattern:** Multi-step operations with rollback capabilities
4. **Container Safety:** Exception-safe operations on standard containers
5. **Exception Specifications:** Using noexcept and conditional noexcept
6. **Best Practices:** Proper exception handling patterns
7. **std::expected (C++23):** Monadic error handling as an alternative to exceptions

## When to Use Each Guarantee

### No Exception Safety
❌ **Avoid when possible**
- Legacy code that can't be modified
- Performance-critical code where exceptions are disabled
- Simple operations where failure is acceptable

### Basic Exception Safety
✅ **Minimum requirement for most code**
- Object invariants are always maintained
- No resource leaks or corruption
- State may change but remains valid
- Most standard library operations provide this

### Strong Exception Safety
✅ **Prefer when possible**
- Critical operations where consistency is essential
- Database transactions
- File system operations
- Complex state modifications
- Copy operations can be expensive

### No-Throw Guarantee
✅ **For essential operations**
- Destructors (never throw)
- Move operations
- Swap operations
- Basic getters/setters
- Memory deallocation

## Performance Characteristics

| Guarantee Level | Performance Cost | Use Case |
|-----------------|------------------|----------|
| No Guarantee | Lowest | Simple operations |
| Basic | Low | Most operations |
| Strong | Medium-High | Critical operations |
| No-Throw | Lowest | Essential operations |

**Performance Tips:**
- Strong guarantee often requires copying state
- Use move semantics to reduce copy costs
- noexcept enables compiler optimizations
- Consider the cost-benefit ratio for your use case

## Common Patterns and Best Practices

### 1. RAII Everywhere
```cpp
class SafeResource {
    std::unique_ptr<Resource> resource_;
public:
    SafeResource() : resource_(std::make_unique<Resource>()) {}
    // Automatic cleanup in destructor
};
```

### 2. Copy-and-Swap Idiom
```cpp
class MyClass {
    std::vector<int> data_;
public:
    void safe_operation(std::vector<int> new_data) {
        // Strong guarantee: either succeeds or leaves original unchanged
        std::swap(data_, new_data);
    }
};
```

### 3. Scope Guards
```cpp
class ScopeGuard {
    std::function<void()> cleanup_;
public:
    ScopeGuard(std::function<void()> cleanup) : cleanup_(cleanup) {}
    ~ScopeGuard() { if (cleanup_) cleanup_(); }
};
```

### 4. Transaction Pattern
```cpp
class Transaction {
    std::vector<std::function<void()>> rollbacks_;
public:
    void add_rollback(std::function<void()> rollback) {
        rollbacks_.push_back(rollback);
    }
    ~Transaction() { /* rollback all */ }
};
```

## Exception Handling Best Practices

### 1. Catch by Const Reference
```cpp
try {
    risky_operation();
} catch (const std::exception& e) {
    // Never catch by value - avoids slicing
    std::cout << e.what() << std::endl;
}
```

### 2. Catch Most Specific First
```cpp
try {
    operation();
} catch (const std::invalid_argument& e) {
    // Handle specific case
} catch (const std::exception& e) {
    // Handle general case
}
```

### 3. Never Let Exceptions Escape Destructors
```cpp
~MyClass() noexcept {
    try {
        cleanup();
    } catch (...) {
        // Log error but don't rethrow
    }
}
```

### 4. Use noexcept Appropriately
```cpp
void never_throws() noexcept {
    // Guaranteed not to throw
}

void conditional_noexcept(int x) noexcept(x > 0) {
    // noexcept only if x > 0
}
```

## Modern C++ Exception Safety Features

### 1. Smart Pointers
- `std::unique_ptr` and `std::shared_ptr` provide automatic cleanup
- Exception-safe resource management
- No manual `delete` calls needed

### 2. Container Exception Safety
- Standard containers provide strong guarantee for single operations
- `push_back`, `insert`, `erase` are exception-safe
- Multi-operation algorithms may need transaction patterns

### 3. Move Semantics
- Efficient state transfer during exception handling
- `std::move` enables copy-elision in rollback scenarios
- Reduces performance cost of strong guarantee

### 4. std::optional and std::expected
- Represent operations that might fail without exceptions
- Exception-safe alternatives for error handling
- C++23 `std::expected` for detailed error information

#### std::expected Monadic Operations
```cpp
// Traditional approach with exceptions
double traditionalApproach(std::string_view filename, double divisor) {
    // Read from file
    auto readResult = readDoubleFromFile(filename);
    if (!readResult.has_value()) {
        throw std::runtime_error("File read failed: " + errorToString(readResult.error()));
    }

    double value = readResult.value();

    // Validate and process
    auto processedResult = validateAndProcess(value);
    if (!processedResult.has_value()) {
        throw std::runtime_error("Processing failed: " + errorToString(processedResult.error()));
    }

    double processed = processedResult.value();

    // Divide
    auto finalResult = divideBy(processed, divisor);
    if (!finalResult.has_value()) {
        throw std::runtime_error("Division failed: " + errorToString(finalResult.error()));
    }

    return finalResult.value();
}

// Monadic approach with std::expected
std::expected<double, std::string> monadicApproach(std::string_view filename, double divisor) {
    return readDoubleFromFile(filename)
        // Transform FileReadError to string immediately
        .or_else([](FileReadError err) -> std::expected<double, std::string> {
            return std::unexpected(errorToString(err));
        })
        // On success, validate and process the value
        .and_then([](double val) -> std::expected<double, std::string> {
            return validateAndProcess(val)
                .or_else([](ProcessingError err) -> std::expected<double, std::string> {
                    return std::unexpected(errorToString(err));
                });
        })
        // On success, divide by divisor
        .and_then([divisor](double val) -> std::expected<double, std::string> {
            return divideBy(val, divisor)
                .or_else([](ProcessingError err) -> std::expected<double, std::string> {
                    return std::unexpected(errorToString(err));
                });
        });
}
```

#### Key std::expected Operations
- **`and_then()`:** Chain successful operations
- **`or_else()`:** Handle errors and potentially recover
- **`transform()`:** Transform successful values
- **`has_value()` / `value()`:** Access successful result
- **`error()`:** Access error information

#### When to Use std::expected vs Exceptions
- **Use std::expected when:**
  - Error handling is part of the normal flow
  - You want composable error handling
  - Performance is critical (no exception overhead)
  - You need type-safe error information
- **Use exceptions when:**
  - Errors are truly exceptional
  - You need non-local error propagation
  - Legacy code compatibility is required

## Testing Exception Safety

### 1. Unit Tests with Exceptions
```cpp
TEST(MyClass, ExceptionSafety) {
    MyClass obj;
    // Test that operations are exception-safe
    EXPECT_THROW(obj.risky_operation(), std::exception);
    // Verify object is still in valid state
    EXPECT_TRUE(obj.is_valid());
}
```

### 2. Mock Exceptions
```cpp
class MockResource {
public:
    MOCK_METHOD(void, operation, (), (override));
    // Configure to throw on specific calls
    EXPECT_CALL(*mock, operation())
        .WillOnce(Throw(std::runtime_error("test")))
        .WillRepeatedly(Return());
};
```

### 3. Fuzz Testing
- Random exception injection during operations
- Verify program remains in valid state
- Test complex interaction scenarios

## Comparison with Other Error Handling

| Method | Exception Safety | Performance | Complexity |
|--------|------------------|-------------|------------|
| Exceptions | High (with guarantees) | Medium | High |
| Error Codes | Manual | Low | Low |
| std::optional | High | Low | Medium |
| std::expected | High | Low | Medium |

## Exceptionless Programming Patterns

In embedded or real-time environments where exceptions are disabled (e.g., `-fno-exceptions`), standard exception handling mechanisms are unavailable. This poses a challenge for constructors, which strictly have no return value and typically rely on exceptions to signal failure.

### The Problem: Constructor Failures
If a constructor encounters an error (e.g., hardware initialization failure) and cannot throw, the object might be left in a "half-baked" or invalid state ("zombie object"). 
Throwing exception from a destructor is undefined behavior. If an exception is thrown during stack unwinding (e.g., during cleanup after another exception), the program will terminate.

### The Solution: Factory Pattern
To handle this safely without exceptions, use a static **Factory Method** that returns a type capable of attempting construction and reporting success or failure.

#### Option A: `std::optional` (Success/Failure)
Make the constructor private and expose a `static std::optional<T> create()` method.

```cpp
class HardwareInterface {
    // Private constructor prevents direct instantiation
    HardwareInterface() { /* ... */ } 
public:
    static std::optional<HardwareInterface> create() {
        if (!init_hardware()) return std::nullopt;
        return HardwareInterface();
    }
};
```

#### Option B: `std::expected` (Rich Error Reporting - C++23)
Use `std::expected<T, E>` to return either the valid object or a specific error code. This is often preferred in modern C++ as it enforces error checking (the "expected" value cannot be accessed without checking validity).

```cpp
class HardwareInterface {
    HardwareInterface() { /* ... */ } // Private constructor
public:
    static std::expected<HardwareInterface, HardwareError> create() {
        if (!init_hardware()) return std::unexpected(HardwareError::InitFailed);
        return HardwareInterface();
    }
};
```

## Advanced Usage Patterns

### 1. Exception-Safe Assignment
```cpp
MyClass& operator=(const MyClass& other) {
    MyClass temp(other);  // Copy construct
    swap(*this, temp);    // No-throw swap
    return *this;         // Strong guarantee
}
```

### 2. Exception-Safe Initialization
```cpp
class ComplexObject {
    std::unique_ptr<Resource1> res1_;
    std::unique_ptr<Resource2> res2_;

public:
    ComplexObject() {
        // If res2_ construction fails, res1_ is automatically cleaned up
        res1_ = std::make_unique<Resource1>();
        res2_ = std::make_unique<Resource2>();
    }
};
```

### 3. Exception-Safe Algorithms
```cpp
template <typename Container, typename Func>
void safe_for_each(Container& c, Func f) {
    for (auto& item : c) {
        try {
            f(item);
        } catch (...) {
            // Log error but continue processing
            std::cout << "Operation failed for item, continuing..." << std::endl;
        }
    }
}
```

Exception safety is fundamental to writing robust, maintainable C++ code. It ensures that your programs behave predictably even in failure scenarios, preventing data corruption and resource leaks. Modern C++ provides excellent tools for achieving exception safety, but it requires careful design and testing.