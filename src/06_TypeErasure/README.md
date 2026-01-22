# Type Erasure

## Overview

Type Erasure is a C++ technique that allows storing objects of different types in a homogeneous container while preserving their polymorphic behavior. The "erasure" happens at compile time - the specific type information is hidden behind a uniform interface.

**The Problem:** Traditional polymorphism requires inheritance from a common base class. What if you want to store different types that don't share a common interface, or you want to avoid the inheritance requirement?

**The Solution: Type Erasure** Create a wrapper that "erases" type information while maintaining a consistent interface through templates and virtual functions.

## How Type Erasure Works

Type erasure typically uses a combination of:

1. **External Polymorphism**: A concept class defines the interface
2. **Template Models**: Concrete implementations for each type
3. **Value Semantics**: Copy and assignment operations
4. **Type Safety**: Compile-time type checking

```cpp
// Concept (interface)
struct Concept {
    virtual ~Concept() = default;
    virtual void operation() = 0;
    virtual std::unique_ptr<Concept> clone() const = 0;
};

// Model template (implementation for any type)
template <typename T>
struct Model : Concept {
    T object_;
    void operation() override { /* use object_ */ }
    std::unique_ptr<Concept> clone() const override {
        return std::make_unique<Model<T>>(object_);
    }
};

// Wrapper (type-erased interface)
class Wrapper {
    std::unique_ptr<Concept> pimpl_;
public:
    template <typename T>
    Wrapper(T obj) : pimpl_(std::make_unique<Model<T>>(std::move(obj))) {}

    void operation() { pimpl_->operation(); }
};
```

### Other examples of Type Erasure

**Hide the Type Behind a Common Interface**
`std::function` is the classic example - it can hold any callable (function, lambda, functor):

```cpp
// These are completely unrelated types
int freeFunction(int x) { return x * 2; }

struct Functor {
    int factor;
    int operator()(int x) const { return x * factor; }
};

auto lambda = [](int x) { return x + 1; };

// But std::function can hold any of them!
std::function<int(int)> func;

func = freeFunction;
std::cout << func(5) << "\n";  // 10

func = Functor{3};
std::cout << func(5) << "\n";  // 15

func = lambda;
std::cout << func(5) << "\n";  // 6
```

**How Type Erasure Works (Simplified):**
```cpp
// Conceptual implementation of type erasure
class AnyPrintable {
    // Internal interface
    struct Concept {
        virtual void print() const = 0;
        virtual ~Concept() = default;
    };
    
    // Templated implementation that wraps any type
    template <typename T>
    struct Model : Concept {
        T value;
        Model(T v) : value(std::move(v)) {}
        void print() const override { std::cout << value; }
    };
    
    std::unique_ptr<Concept> ptr;
    
public:
    // Constructor accepts any printable type
    template <typename T>
    AnyPrintable(T value) : ptr(std::make_unique<Model<T>>(std::move(value))) {}
    
    void print() const { ptr->print(); }
};

// Usage
AnyPrintable a = 42;
AnyPrintable b = std::string("Hello");
AnyPrintable c = 3.14;

a.print();  // 42
b.print();  // Hello
c.print();  // 3.14
```

## Examples in the Standard Library

- `std::function`: Type-erases callable objects
- `std::any`: Type-erases any type (C++17)
- `std::shared_ptr`: Type-erases deleter and allocator types


**Standard Library Type Erasure:**
| Class | Erases |
|-------|--------|
| `std::function<R(Args...)>` | Any callable with matching signature |
| `std::any` | Any copyable type |
| `std::variant<Types...>` | One of the listed types (not true erasure, but similar use) |


## Sample Output

```
Running Type Erasure Sample...

=== Basic Type Erasure Example ===
Drawing all shapes:
Drawing circle with radius 3.5
Drawing square with side 2
Drawing triangle with base 4 and height 3

Copying and modifying:
Original: Drawing circle with radius 3.5
Copy: Drawing circle with radius 3.5

=== Function Object Type Erasure ===
Calling all functions:
Called from lambda!
Called from function pointer!
Called from functor!

=== Advanced Type Erasure with Multiple Operations ===
Initial state:
Counter: 5
Accumulator: 10

After processing:
Counter: 6
Accumulator: 20

=== Traditional Inheritance Approach ===
Shape: Circle - Drawing circle with radius 5
Shape: Square - Drawing square with side 4
Shape: Triangle - Drawing triangle with base 3 and height 4
Problem: All objects must inherit from Drawable

=== Type Erasure Approach ===
Shape: Circle - Drawing circle with radius 5
Shape: Square - Drawing square with side 4
Shape: Triangle - Drawing triangle with base 3 and height 4
Advantage: No inheritance requirement, any type works!

=== Performance Considerations ===
Type Erasure Benefits:
- Homogeneous storage of heterogeneous types
- No inheritance requirements
- Runtime polymorphism without virtual inheritance
- Can work with third-party types

Type Erasure Costs:
- Dynamic allocation (heap usage)
- Virtual function call overhead
- Copy operations require deep cloning
- Type information is 'erased' at compile time

Type erasure demonstration completed!
```

## When to Use Type Erasure

✅ **Good for:**
- Libraries that need to accept any type with a specific interface
- Storing heterogeneous objects in containers
- Creating generic callbacks or event handlers
- Working with third-party types without modification
- Implementing value semantics for polymorphic objects

❌ **Not good for:**
- Performance-critical code (virtual calls + heap allocation)
- When you need access to the actual type at runtime
- Simple cases where inheritance works fine
- When copy operations are very expensive

## Implementation Details

This sample demonstrates:

1. **Basic Type Erasure**: Drawable objects stored homogeneously
2. **Function Objects**: Type-erased callables (like `std::function`)
3. **Multiple Operations**: Complex interfaces with several methods
4. **Comparison**: Type erasure vs traditional inheritance
5. **Copy Semantics**: Proper deep copying through cloning

The pattern uses the "external polymorphism" approach where the polymorphic behavior is implemented outside the types being stored, rather than through inheritance.

## Performance Characteristics

**Advantages:**
- Type-safe at compile time
- No inheritance requirements
- Works with any type that satisfies the concept
- Value semantics (copyable, assignable)

**Disadvantages:**
- Heap allocation for each object
- Virtual function call overhead
- Larger memory footprint
- Copying requires deep cloning

## Alternatives

- **Inheritance**: When all types can inherit from a common base
- `std::variant`: When you have a fixed set of types
- `std::any`: When you need to store any type (type erasure for single objects)
- Templates: When types are known at compile time

## Type Erasure vs CRTP: Choosing the Right Tool

Both Type Erasure and CRTP are alternatives to traditional virtual functions, but they solve **fundamentally different problems**:

| Aspect | Type Erasure | CRTP |
|--------|-------------|------|
| **Polymorphism** | Runtime (dynamic) | Compile-time (static) |
| **Heterogeneous containers** | ✅ Yes | ❌ No |
| **Performance** | Virtual call + heap allocation | Zero overhead |
| **Type must be known at** | Runtime | Compile time |
| **Memory** | Heap (dynamic allocation) | Stack (no allocation) |
| **Use case** | Storing different types uniformly | Mixins, static dispatch |

### The Key Question

> **"Do I need to store different types in the same container?"**

- **YES** → Use **Type Erasure** (or `std::variant` if types are fixed)
- **NO** → Use **CRTP** for better performance

### Side-by-Side Comparison

**Type Erasure: Heterogeneous Storage**
```cpp
// Store ANY callable - types determined at runtime
std::vector<std::function<void()>> callbacks;
callbacks.push_back([]() { std::cout << "Lambda"; });
callbacks.push_back(MyFunctor{});
callbacks.push_back(&freeFunction);

for (auto& cb : callbacks) cb();  // All work uniformly
```

**CRTP: Compile-Time Mixin**
```cpp
// Add functionality without virtual calls - types known at compile time
template <typename T>
struct Printable {
    void print() const {
        std::cout << static_cast<const T&>(*this).toString();
    }
};

struct Person : Printable<Person> {
    std::string name;
    std::string toString() const { return name; }
};

Person p{"Alice"};
p.print();  // No virtual call, resolved at compile time
```

### When Type Erasure Wins

| Scenario | Why Type Erasure |
|----------|------------------|
| **Event handlers** | Unknown callbacks registered at runtime |
| **Plugin systems** | Plugins implement interface, stored together |
| **Undo/Redo stack** | Different command types in one deque |
| **Serialization** | Different types written to same stream |
| **std::function usage** | Store any callable with same signature |

### When CRTP Wins

| Scenario | Why CRTP |
|----------|----------|
| **Operator generation** | Auto-generate `>`, `<=`, `>=` from `<` |
| **Instance counting** | Track objects without virtual overhead |
| **Static interfaces** | Enforce interface at compile time |
| **Expression templates** | Zero-overhead lazy evaluation |
| **Singleton pattern** | Reusable singleton mixin |

### Hybrid Approach

Sometimes you need both! Use CRTP for the internal implementation and Type Erasure for the external interface:

```cpp
// Internal: CRTP for code reuse
template <typename Derived>
struct ShapeBase {
    double area() const { return static_cast<const Derived&>(*this).computeArea(); }
};

struct Circle : ShapeBase<Circle> {
    double radius;
    double computeArea() const { return 3.14159 * radius * radius; }
};

// External: Type Erasure to store different shapes
class AnyShape { /* type-erased wrapper */ };
std::vector<AnyShape> shapes;
shapes.push_back(Circle{5.0});
shapes.push_back(Rectangle{3, 4});

// Use shapes uniformly
for (auto& shape : shapes) 
    std::cout << shape.area() << "\n";
```