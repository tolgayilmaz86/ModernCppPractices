# Variant and Visitor Pattern

## Overview

The Variant and Visitor pattern combines `std::variant` (C++17) with the visitor pattern to provide type-safe, compile-time polymorphism. This approach offers an alternative to inheritance-based polymorphism when you have a fixed set of types and want to perform different operations on each type.

**The Problem:** Traditional inheritance requires a common base class and virtual functions. What if you want type-safe operations on a discriminated union without the overhead of inheritance?

**The Solution: Variant + Visitor** Use `std::variant` to store different types safely, and the visitor pattern to perform type-specific operations in a type-safe manner.

## How Variant + Visitor Works

1. **Define the Variant:** `std::variant<Type1, Type2, ...>` can hold any of the specified types
2. **Create Visitors:** Functions or functors that handle each type in the variant
3. **Use std::visit:** Applies the visitor to the variant's current value
4. **Type Safety:** All operations are checked at compile time

```cpp
using Shape = std::variant<Circle, Square, Triangle>;

struct DrawVisitor {
    void operator()(const Circle& c) { /* draw circle */ }
    void operator()(const Square& s) { /* draw square */ }
    void operator()(const Triangle& t) { /* draw triangle */ }
};

Shape shape = Circle{5.0};
std::visit(DrawVisitor{}, shape);  // Type-safe dispatch
```

## Sample Output

```
Running Variant Visitor Sample...

=== Basic Variant Usage ===
Variant sizes: 32 bytes

=== Custom Visitor Class ===
Processing circle with radius: 5, area: 78.5397
Processing polygon with 6 sides
Processing named shape: triangle
Empty shape (monostate)

=== Overloaded Lambdas (Simple Visitors) ===
Simple circle: 5
Simple polygon: 6 sides

=== Advanced Visitor with State ===
Circle area: 19.6349
Polygon area: 40
Polygon area: 80
Named shape 'hexagon' area: 50

Summary: 4 shapes processed, total area: 189.635

=== Error Handling ===
Caught error: Radius must be positive
Valid circle with radius: 3

=== Traditional Inheritance Approach ===
Drawing circle
Area: 78.5397
Drawing polygon
Area: 60
Problem: Requires inheritance hierarchy
Problem: Cannot easily add new operations without modifying classes

=== Variant + Visitor Approach ===
Processing circle with radius: 5, area: 78.5397
Processing polygon with 6 sides
Processing named shape: triangle
Advantage: No inheritance required
Advantage: Easy to add new operations (just new visitors)
Advantage: Type-safe at compile time

=== Multiple Visitors on Same Data ===
circle(r=3) -> area: 28.2743
polygon(sides=4) -> area: 40
polygon(sides=5) -> area: 50
named(square) -> area: 50
Total area: 168.274

=== Direct Access with std::get and std::holds_alternative ===
Shape holds a double (circle radius): 5
Shape holds an int (polygon sides): 6
Caught std::bad_variant_access: bad variant access
This is expected - we tried to get double from string variant
Safe access with get_if - radius: 4.5

=== Memory Layout and Performance ===
sizeof(std::variant<char, short>): 4 bytes
sizeof(std::variant<std::string, std::vector<int>>): 32 bytes
sizeof(std::string): 32 bytes
sizeof(std::vector<int>): 24 bytes

Variant size is max(alternatives) + index overhead
Small variants are efficient, large variants may waste space

=== Advanced Lambda Visitors ===
Shape: circle(r=3.000000)
Shape: polygon(sides=4)
Shape: named(pentagon)
Shape: empty

Advanced lambdas can use constexpr and type traits for complex logic

=== Performance Comparison: Variant vs Inheritance ===
Variant approach: 1250 microseconds
Total area calculated: 1.5708e+11

Key advantages of variants:
- No heap allocation for small objects
- Better cache locality
- No virtual function overhead
- Compile-time polymorphism resolution
```

## Key Components

### 1. std::variant
- **Type-safe union** that can hold one of several specified types
- **Compile-time type checking** prevents invalid operations
- **Fixed memory layout** - size is max of all alternative types
- **Default constructible** with first type, or use `std::monostate` for empty

### 2. Visitor Pattern
- **Overloaded call operator** for each type in the variant
- **Type-specific operations** without runtime type checks
- **Can maintain state** across multiple visits
- **Exception safety** through proper error handling

### 3. std::visit
- **Type-safe dispatch** to the appropriate visitor function
- **Compile-time resolution** of which overload to call
- **Works with any callable** (functions, lambdas, functors)
- **Returns the result** of the visitor call

## Implementation Details

This sample demonstrates:

1. **Basic Variant Usage**: Creating and storing different types
2. **Custom Visitor Classes**: Complex operations with state
3. **Overloaded Lambdas**: Simple visitors using C++17 features
4. **Error Handling**: Type-safe validation and exception handling
5. **Multiple Visitors**: Different operations on the same data
6. **Direct Access Methods**: Using `std::get`, `std::holds_alternative`, and `std::get_if`
7. **Memory Layout**: Understanding variant storage characteristics
8. **Advanced Lambda Visitors**: Using `constexpr` and type traits
9. **Performance Comparison**: Variant vs inheritance approaches

## When to Use Variant + Visitor

✅ **Good for:**
- Fixed set of types known at compile time
- Type-safe discriminated unions
- Avoiding inheritance hierarchies
- Performance-critical code with small type sets
- Adding new operations without modifying existing types
- Strong type safety requirements

❌ **Not good for:**
- Dynamic type sets (types added at runtime)
- Large number of types (complexity grows)
- Simple cases where inheritance works fine
- When you need runtime type identification
- Third-party types you cannot modify

## Performance Characteristics

**Advantages:**
- **No virtual function calls** (compile-time dispatch)
- **Fixed memory layout** (no heap allocation for small types)
- **Better cache locality** than inheritance
- **Type safety** at compile time
- **No inheritance overhead**

**Disadvantages:**
- **Must know all types** at compile time
- **Cannot add types** without recompilation
- **std::visit overhead** (though minimal)
- **Larger binary size** with many types
- **Complex syntax** for simple cases

## Comparison with Other Approaches

| Approach | Inheritance | Variant + Visitor | std::any | Templates |
|----------|-------------|-------------------|----------|-----------|
| Type Safety | Runtime | Compile-time | Runtime | Compile-time |
| Performance | Virtual calls | Direct calls | Type checks | Monomorphized |
| Flexibility | High | Medium | High | Low |
| Complexity | Medium | Medium | Low | High |
| Memory | Vtable ptr | Fixed size | Dynamic | Optimized |

## Advanced Usage Patterns

### 1. Recursive Variants
```cpp
using Expr = std::variant<int, std::unique_ptr<struct BinaryOp>>;
```

### 2. Visitor with Return Types
```cpp
auto result = std::visit([](auto&& arg) -> ReturnType {
    return process(arg);
}, variant);
```

### 3. Generic Visitors
```cpp
template <typename... Visitors>
auto make_visitor(Visitors&&... visitors) {
    return overloaded{std::forward<Visitors>(visitors)...};
}
```

## Best Practices

1. **Use std::monostate** for variants that can be empty
2. **Keep variant sizes small** - prefer references for large types
3. **Use overloaded lambdas** for simple visitors
4. **Custom visitor classes** for complex stateful operations
5. **Handle all types** in visitors to avoid compilation errors
6. **Consider inheritance** for very simple cases
7. **Use const references** in visitors when possible

## Modern C++ Integration

The Variant + Visitor pattern integrates well with other Modern C++ features:

- **Smart pointers** for complex types in variants
- **Move semantics** for efficient variant operations
- **constexpr** for compile-time computations
- **Concepts** (C++20) for constraining visitor parameters
- **Coroutines** for async visitor operations

This pattern represents a powerful tool in the Modern C++ toolbox for type-safe, efficient polymorphic operations without inheritance.