# SFINAE (Substitution Failure Is Not An Error)

## Overview

SFINAE stands for "Substitution Failure Is Not An Error" and is a C++ programming technique that allows templates to be specialized based on the ability of types to compile with certain expressions. It's a key mechanism behind template metaprogramming and enables compile-time introspection and conditional compilation.
**The Problem:**
You want to write a template function that only works with certain types (e.g., only for types with a `.size()` method).

**What is SFINAE?**
When the compiler tries to instantiate a template, if substituting the type causes an invalid expression, the compiler doesn't error - it just removes that overload from consideration. This lets you create conditional function overloads.

```cpp
// Example: Function that only works for types with .size()

// This version is selected for types WITH .size()
template <typename T>
auto getSize(const T& container) -> decltype(container.size()) {
    return container.size();
}

// This version is selected for types WITHOUT .size() (like arrays)
template <typename T, size_t N>
size_t getSize(const T (&arr)[N]) {
    return N;
}

std::vector<int> vec = {1, 2, 3};
int arr[] = {1, 2, 3, 4, 5};

getSize(vec);  // Uses first overload: vec.size() = 3
getSize(arr);  // Uses second overload: N = 5
```

**`std::enable_if` - Explicit SFINAE Control**
```cpp
#include <type_traits>

// Only enable for integral types (int, long, char, etc.)
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
doubleValue(T value) {
    return value * 2;
}

// Only enable for floating point types
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
doubleValue(T value) {
    return value * 2.0;
}

doubleValue(5);      // Calls integral version
doubleValue(3.14);   // Calls floating point version
// doubleValue("hi"); // Compile error: no matching function
```

**C++17 Improvement: `if constexpr`**
```cpp
template <typename T>
auto doubleValue(T value) {
    if constexpr (std::is_integral_v<T>) {
        return value * 2;        // Only compiled for integers
    } else if constexpr (std::is_floating_point_v<T>) {
        return value * 2.0;      // Only compiled for floats
    } else {
        static_assert(false, "Unsupported type");
    }
}
```

**C++20: Just Use Concepts!**
```cpp
template <std::integral T>
T doubleValue(T value) { return value * 2; }

template <std::floating_point T>
T doubleValue(T value) { return value * 2.0; }

// Much cleaner! Error messages are also better.
```

---

## Key Concepts

- **Template Overload Resolution**: When multiple template overloads exist, SFINAE allows some to be discarded if substitution fails
- **Compile-Time Introspection**: Determine properties of types at compile time
- **Conditional Compilation**: Enable/disable code based on type traits
- **Metaprogramming**: Foundation for advanced template techniques

## Real-World Example: Type Traits and Function Overloading

In this example, we demonstrate SFINAE by creating overloaded functions that behave differently based on whether a type has a specific member function or supports certain operations.

### Code Example

See `SFINAESample.cpp` for the implementation.

### Usage

The example shows:
- Checking if a type has a `begin()` method (container detection)
- Checking if types are arithmetic
- Using `std::enable_if` for conditional compilation
- Function overloading based on type properties

This pattern is commonly used in:
- Type traits libraries
- Generic algorithms
- Template specialization
- Library design for handling different types gracefully