# CRTP (Curiously Recurring Template Pattern)

## Overview

CRTP is an idiom in C++ where a class `Derived` inherits from a template class `Base<Derived>`. This allows the base class to know about the derived class at compile time, enabling static polymorphism and mixin classes.

**The Problem:** Traditional inheritance requires virtual functions for polymorphism, which have runtime overhead. Sometimes we want compile-time polymorphism or to add functionality to classes without inheritance hierarchies.

**The Solution: CRTP** allows base classes to call derived class methods statically, providing compile-time polymorphism and reusable mixins.

## Key Benefits

- **Zero runtime overhead**: All calls are resolved at compile time
- **Static polymorphism**: No virtual function tables
- **Mixin pattern**: Add functionality to any class without complex inheritance
- **Type safety**: Compile-time type checking

## Common Use Cases

### 1. Counter Mixin
Track the number of instances of a class:

```cpp
template <typename T>
class Counter {
private:
    static size_t count;
public:
    Counter() { ++count; }
    ~Counter() { --count; }
    static size_t getCount() { return count; }
};

class Widget : public Counter<Widget> {
    // Widget now has counting functionality
};
```

### 2. Comparison Operators
Automatically generate all comparison operators from just `<`:

```cpp
template <typename T>
class Comparable {
public:
    friend bool operator==(const T& lhs, const T& rhs) {
        return !(lhs < rhs) && !(rhs < lhs);
    }
    // Other operators defined similarly...
};

class MyClass : public Comparable<MyClass> {
public:
    bool operator<(const MyClass& other) const {
        // Only need to implement this one
    }
    // ==, !=, <=, >, >= all work automatically!
};
```

### 3. Clone Functionality
Polymorphic copying without virtual functions:

```cpp
template <typename T>
class Cloneable {
public:
    std::unique_ptr<T> clone() const {
        return std::unique_ptr<T>(static_cast<T*>(this->doClone()));
    }
private:
    virtual Cloneable* doClone() const = 0;
};

class Shape : public Cloneable<Shape> {
private:
    Shape* doClone() const override { return new Shape(*this); }
};
```

## Implementation Details

In this sample, we demonstrate three CRTP mixins:

1. **Counter**: Tracks instance counts statically
2. **Comparable**: Provides all comparison operators from just `<`
3. **Cloneable**: Enables polymorphic copying

The pattern works because:
- The base class template parameter `T` is the derived class
- Base class methods can call `static_cast<T*>(this)` safely
- All type information is available at compile time

## When to Use CRTP

✅ **Good for:**
- Mixin classes (adding functionality to existing classes)
- Compile-time polymorphism
- Avoiding virtual function overhead
- Type-safe generic programming

❌ **Not good for:**
- Runtime polymorphism (use virtual functions)
- When you need to store heterogeneous objects
- Complex inheritance hierarchies

## Runtime vs Compile-Time Polymorphism: Decision Guide

### Use Compile-Time Polymorphism (CRTP/Templates) When:

| Scenario | Real-World Example |
|----------|-------------------|
| **Types known at compile time** | A math library where `Vector<float>` and `Vector<double>` are distinct types |
| **Performance critical** | Game engine entity components updated 60+ times/second |
| **No heterogeneous collections** | Numeric algorithms that always work on same type |
| **Code bloat is acceptable** | Embedded systems with specific hardware types |

**Example: High-Performance Math Library**
```cpp
// CRTP for zero-overhead expression templates
template <typename Derived>
struct VectorExpr {
    double operator[](size_t i) const { 
        return static_cast<const Derived&>(*this)[i]; 
    }
};

struct Vector : VectorExpr<Vector> { /*...*/ };

// v1 + v2 + v3 compiles to single loop - no temporaries!
```

### Use Runtime Polymorphism (Virtual Functions) When:

| Scenario | Real-World Example |
|----------|-------------------|
| **Types determined at runtime** | Plugin systems, user-loaded modules |
| **Heterogeneous collections** | GUI widgets: buttons, sliders, labels in one container |
| **Type changes at runtime** | State machines, strategy pattern |
| **Binary compatibility needed** | Shared libraries with stable ABI |

**Example: GUI Framework**
```cpp
// Virtual functions - types unknown until user interacts
class Widget {
public:
    virtual void draw() = 0;
    virtual void handleClick(int x, int y) = 0;
};

std::vector<std::unique_ptr<Widget>> widgets;
widgets.push_back(loadWidgetFromFile("button.xml"));  // Type unknown at compile time
widgets.push_back(createWidgetFromUserInput());       // User decides at runtime

for (auto& w : widgets) w->draw();  // Must be virtual
```

### Quick Decision Flowchart

```
Do you need to store different types in one container?
    │
    ├─ YES → Do types change at runtime (plugins, user input)?
    │           ├─ YES → Use VIRTUAL FUNCTIONS
    │           └─ NO  → Consider std::variant + std::visit
    │
    └─ NO  → Is performance critical (millions of calls)?
                ├─ YES → Use CRTP / Templates
                └─ NO  → Either works; prefer virtual for simplicity
```

### Real-World Comparison

| Use Case | Best Choice | Why |
|----------|-------------|-----|
| **Game physics engine** | CRTP | Called every frame, types known (RigidBody, SoftBody) |
| **Drawing app shapes** | Virtual | User creates shapes at runtime, stored in layers |
| **JSON parser** | CRTP | Types (Object, Array, String) known, performance matters |
| **Plugin architecture** | Virtual | Plugins loaded at runtime, binary compatibility needed |
| **STL algorithms** | Templates | Types known, zero overhead, works with any iterator |
| **Device drivers** | Virtual | Hardware detected at runtime, ABI stability required |


## Sample Output

```
Running CRTP Sample...

=== Counter Mixin ===
Initial Widget count: 0
After creating w1: 1
After creating w2 and w3: 3
Inside scope with w4: 4
After w4 goes out of scope: 3

=== Comparable Mixin ===
w1 (10) < w2 (20): 1
w1 (10) == w1 (10): 1
w2 (20) > w3 (15): 1
w1 (10) != w2 (20): 1

=== Cloneable Mixin ===
Original circle area: 78.5397
Original rectangle area: 24
Cloned circle area: 78.5397
Cloned rectangle area: 24

CRTP demonstration completed!
```</content>
<parameter name="filePath">d:\repos\ModernCppPractices\src\03_CRTP\README.md