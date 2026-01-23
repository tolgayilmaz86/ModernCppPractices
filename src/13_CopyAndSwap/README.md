# Copy and Swap Idiom

## Overview

The Copy and Swap Idiom is a C++ idiom for implementing assignment operators safely and efficiently. It provides strong exception safety and simplifies code by reusing copy constructors and swap functions.

**The Problem:** Implementing assignment operators correctly is error-prone and can lead to resource leaks or inconsistent state if exceptions occur during assignment.
Writing exception-safe copy assignment operators is tricky. You need to handle self-assignment, clean up old resources, and not leak if an exception occurs.

```cpp
// ❌ BAD: Not exception-safe!
Buffer& Buffer::operator=(const Buffer& other) {
    if (this == &other) return *this;  // Self-assignment check
    
    delete[] data;                      // Clean up old data
    size = other.size;
    data = new int[size];               // If this throws, object is broken!
    std::copy(other.data, other.data + size, data);
    return *this;
}
```

**The Solution:** Use copy construction to create a temporary copy, then swap it with the current object. This ensures either the assignment succeeds completely or fails completely.

## How Copy and Swap Works

1. **Copy Construction:** Create a temporary copy of the right-hand side (if this throws, original is unchanged)
2. **Swap:** Exchange the contents of the current object with the temporary (noexcept - can't fail)
3. **Destruction:** The temporary (containing the old state) is destroyed
4. Return *this for chaining
   
```cpp
class Resource {
    int* data_;
    std::vector<int> values_;

public:
    // Copy constructor
    Resource(const Resource& other) : data_(new int(*other.data_)), values_(other.values_) {}

    // Copy and swap assignment
    Resource& operator=(Resource other) noexcept {  // Pass by value!
        swap(*this, other);  // Swap with temporary
        return *this;        // Temporary destroyed, old state cleaned up
    }

    // Swap function
    friend void swap(Resource& first, Resource& second) noexcept {
        using std::swap;
        swap(first.data_, second.data_);
        swap(first.values_, second.values_);
    }
};
```

## Sample Output

```
Running Copy and Swap Idiom Sample...

=== Copy and Swap Idiom ===
Creating resource a:
Resource constructed with value 42

Creating resource b:
Resource constructed with value 100

Before assignment: a.value = 42, b.value = 100

Assigning b to a (copy and swap):
Resource copy-constructed
Resource copy-and-swap assignment
Resources swapped

After assignment: a.value = 100, b.value = 100
a.values: 3
b.values: 1 2

=== Exception Safety ===
Copy and swap provides strong exception safety:
- If copy construction fails, original object is unchanged
- If swap fails (unlikely), temporary is cleaned up
- Assignment either succeeds completely or fails completely

=== Best Practices ===
- Use copy and swap for assignment operators
- Make swap noexcept for efficiency
- Implement both copy and move constructors
- Use ADL (Argument Dependent Lookup) for swap
- Consider using std::swap for standard types

Copy and Swap Idiom demonstration completed!
```

## Key Components

### 1. Copy Constructor
- **Creates a deep copy** of the right-hand side
- **Used to create the temporary** in assignment

### 2. Swap Function
- **Exchanges contents** of two objects
- **Should be noexcept** for efficiency
- **Uses ADL** for proper overload resolution

### 3. Assignment Operator
- **Takes parameter by value** (creates copy)
- **Swaps with the temporary**
- **Returns *this** for chaining

## Implementation Details

This sample demonstrates:

1. **Copy and Swap Assignment:** Safe and efficient assignment implementation
2. **Exception Safety:** Strong guarantee through copy and swap
3. **Best Practices:** Modern C++ patterns for resource management

## When to Use Copy and Swap

### ✅ **Use copy and swap when:**
- Implementing assignment operators for resource-owning classes
- You need strong exception safety
- Your class has complex resource management
- You want to avoid code duplication between copy and move assignment

### ❌ **Don't use copy and swap when:**
- Assignment is trivial (just copy members)
- You need custom assignment logic
- Performance is critical and copy can be avoided

## Performance Characteristics

| Method | Exception Safety | Performance | Code Complexity |
|--------|------------------|-------------|-----------------|
| Manual Assignment | Weak/Basic | Fast | High |
| Copy and Swap | Strong | Medium | Low |

**Performance Tips:**
- Copy and swap requires one extra copy
- For expensive copies, consider move semantics
- Swap should be noexcept for optimal performance

## Common Patterns and Best Practices

### 1. Basic Copy and Swap
```cpp
class MyClass {
    std::vector<int> data_;
public:
    MyClass& operator=(MyClass other) noexcept {
        swap(*this, other);
        return *this;
    }
    friend void swap(MyClass& a, MyClass& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
    }
};
```

### 2. Self-Assignment Safe
```cpp
MyClass& operator=(MyClass other) noexcept {
    // other is a copy, so self-assignment is safe
    swap(*this, other);
    return *this;
}
```

### 3. With Move Semantics
```cpp
class Resource {
    std::unique_ptr<int> data_;
public:
    Resource& operator=(Resource other) noexcept {
        swap(*this, other);
        return *this;
    }
    // Move constructor handles the copy
};
```

## Exception Safety Levels

### 1. No Guarantee
- Assignment may leave object in invalid state

### 2. Basic Guarantee
- Object invariants are preserved, but state may change

### 3. Strong Guarantee
- Assignment either succeeds completely or fails completely
- **Copy and swap provides strong guarantee**

### 4. No-Throw Guarantee
- Assignment never throws (rare, usually for simple types)

## Testing Copy and Swap

### 1. Self-Assignment
```cpp
TEST(MyClass, SelfAssignment) {
    MyClass obj(42);
    obj = obj;  // Should not crash
    EXPECT_EQ(obj.getValue(), 42);
}
```

### 2. Exception Safety
```cpp
TEST(MyClass, ExceptionSafety) {
    MyClass obj1(42);
    MyClass obj2(100);
    // Test that assignment either succeeds or leaves obj1 unchanged
    EXPECT_NO_THROW(obj1 = obj2);
    EXPECT_EQ(obj1.getValue(), 100);
}
```

### 3. Performance
```cpp
TEST(MyClass, AssignmentPerformance) {
    MyClass large_obj = create_large_object();
    MyClass copy = large_obj;
    // Measure time for assignment
    auto start = std::chrono::high_resolution_clock::now();
    large_obj = copy;
    auto end = std::chrono::high_resolution_clock::now();
    // Assert reasonable performance
}
```

## Advanced Usage Patterns

### 1. Copy and Swap with Inheritance
```cpp
class Base {
public:
    virtual Base& operator=(Base other) {
        swap(*this, other);
        return *this;
    }
    virtual void swap(Base& other) noexcept = 0;
};
```

### 2. Non-Copyable Resources
```cpp
class UniqueResource {
    std::unique_ptr<int> data_;
public:
    UniqueResource& operator=(UniqueResource other) noexcept {
        // Move assignment through copy and swap
        swap(*this, other);
        return *this;
    }
};
```

The Copy and Swap Idiom is a cornerstone of exception-safe C++ programming. It simplifies assignment operator implementation while providing strong exception safety guarantees.