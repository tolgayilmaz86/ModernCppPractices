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

## CRTP vs Type Erasure: Choosing the Right Tool

Both CRTP and Type Erasure are alternatives to virtual functions, but they solve **fundamentally different problems**:

| Aspect | CRTP | Type Erasure |
|--------|------|-------------|
| **Polymorphism** | Compile-time (static) | Runtime (dynamic) |
| **Heterogeneous containers** | ❌ No | ✅ Yes |
| **Performance** | Zero overhead | Virtual call + heap allocation |
| **Type must be known at** | Compile time | Runtime |
| **Memory** | Stack (no allocation) | Heap (dynamic allocation) |
| **Use case** | Mixins, static dispatch | Storing different types uniformly |

### When to Use CRTP

```cpp
// CRTP: All types known at compile time, no container needed
template <typename T>
struct Comparable {
    bool operator>(const T& other) const {
        return other < static_cast<const T&>(*this);
    }
};

struct Price : Comparable<Price> {
    double value;
    bool operator<(const Price& other) const { return value < other.value; }
};

// Usage: No heap allocation, no virtual calls
Price p1{10.0}, p2{20.0};
bool result = p1 > p2;  // Resolved at compile time
```

**Use CRTP when:**
- Adding reusable functionality to classes (mixins)
- You need maximum performance (no virtual calls)
- Types are known at compile time
- You don't need to store different types together

### When to Use Type Erasure

```cpp
// Type Erasure: Store ANY drawable in one container
class Drawable {  
public:
    virtual void draw() = 0;
    virtual ~Drawable() = default;
};

std::vector<std::unique_ptr<Drawable>> shapes;  // Heterogeneous container!
shapes.push_back(std::make_unique<Circle>(5.0));     // Circle
shapes.push_back(std::make_unique<Rectangle>(3, 4)); // Rectangle  
shapes.push_back(std::make_unique<CustomShape>());   // Third-party type - no inheritance needed!

for (auto& shape : shapes) shape->draw();  // Works uniformly
```

**Use Type Erasure when:**
- Storing different types in the same container
- Working with third-party types you can't modify
- You need value semantics (copyable polymorphic objects)
- Types might be determined at runtime

### Real-World Decision Examples

| Scenario | Best Choice | Why |
|----------|-------------|-----|
| **Logging with different backends** | Type Erasure | Store File, Console, Network loggers in one vector |
| **Adding serialization to classes** | CRTP | Mixin pattern, no container needed |
| **Event system callbacks** | Type Erasure | Store any callable uniformly (`std::function`) |
| **Operator overloading helpers** | CRTP | Generate operators at compile time |
| **Plugin system** | Type Erasure | Plugins loaded at runtime, stored together |
| **Static singleton pattern** | CRTP | Compile-time, no virtual overhead |


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