# Tag Dispatching

## Overview

Tag dispatching is a compile-time technique in Modern C++ that selects function overloads based on type tags, enabling optimal algorithm selection and specialization. It is widely used in the STL (e.g., iterator categories) and is a best practice for customizing behavior without SFINAE or enable_if.

**The Problem:** How to select the most efficient algorithm for a type (e.g., random access vs. input iterator) at compile time.

**The Solution:** Use tag types and overloads to dispatch to the correct implementation.

## How Tag Dispatching Works

1. **Tag Types:** Empty structs representing categories (e.g., `input_iterator_tag`, `random_access_iterator_tag`)
2. **Overload Functions:** Provide different implementations for each tag
3. **Dispatch Function:** Selects the tag based on type traits and calls the correct overload

```cpp
struct input_iterator_tag {};
struct random_access_iterator_tag : input_iterator_tag {};

template <typename Iterator>
void advance_impl(Iterator& it, int n, input_iterator_tag) {
    // Linear advance
}

template <typename Iterator>
void advance_impl(Iterator& it, int n, random_access_iterator_tag) {
    // Fast advance
}

template <typename Iterator>
void advance(Iterator& it, int n) {
    using category = /* deduce tag type */;
    advance_impl(it, n, category{});
}
```

## Sample Output

```
Running Tag Dispatching Sample...

=== Tag Dispatching: Iterator Advance Example ===
Advancing pointer (random access):
advance_impl: random_access_iterator_tag (fast advance)
Pointer now points to: 4
Advancing vector iterator (input):
advance_impl: input_iterator_tag (linear advance)
Iterator now points to: 30

=== Overload Resolution Example ===
process(int)
process(double)
process(string)

=== Tag Dispatching Best Practices ===
Use tag dispatching to select optimal algorithms at compile time.
Prefer tag dispatching over SFINAE for simple cases.
Use standard tags (std::input_iterator_tag, std::random_access_iterator_tag) when possible.

Custom algorithm dispatch example:
compute(fast_tag): Fast algorithm for value 100
compute(slow_tag): Slow algorithm for value 100

Tag dispatching demonstration completed!
```

## Key Components

### 1. Tag Types
- **Purpose:** Represent categories or traits at compile time
- **Implementation:** Empty structs (e.g., `struct fast_tag {}`)

### 2. Overload Functions
- **Purpose:** Provide specialized implementations for each tag
- **Implementation:** Overload on tag type as last parameter

### 3. Dispatch Function
- **Purpose:** Selects the tag and calls the correct overload
- **Implementation:** Uses type traits or user input to select tag

## Implementation Details

This sample demonstrates:

1. **Iterator Tag Dispatching:** Selects advance algorithm based on iterator type
2. **Overload Resolution:** Shows how overloads are selected by argument type
3. **Custom Algorithm Dispatch:** Uses tags to select between fast/slow algorithms
4. **Best Practices:** When and how to use tag dispatching

## When to Use Tag Dispatching

### ✅ **Use tag dispatching when:**
- Selecting algorithms based on type traits (e.g., iterator category)
- Customizing behavior for user-defined types
- Avoiding SFINAE/enable_if for simple cases
- Specializing STL-like algorithms

### ❌ **Don't use tag dispatching when:**
- Complex trait logic (prefer concepts or SFINAE)
- Run-time selection is needed
- Tag types are not meaningful for the domain

## Performance Characteristics

| Method | Compile-Time | Run-Time | Complexity |
|--------|--------------|----------|------------|
| Tag Dispatching | Yes | Zero | Low |
| SFINAE/enable_if | Yes | Zero | Medium |
| Virtual Dispatch | No | Yes | High |

**Performance Tips:**
- Tag dispatching incurs zero run-time overhead
- Prefer tag dispatching for simple, type-based selection
- Use standard tags when possible for STL compatibility

## Common Patterns and Best Practices

### 1. STL Iterator Tag Dispatching
```cpp
template <typename Iterator>
void advance(Iterator& it, int n) {
    using category = typename std::iterator_traits<Iterator>::iterator_category;
    advance_impl(it, n, category{});
}
```

### 2. Custom Tag Types
```cpp
struct fast_tag {};
struct slow_tag {};

template <typename T>
void compute(T value, fast_tag) { /* ... */ }

template <typename T>
void compute(T value, slow_tag) { /* ... */ }
```

### 3. Tag-Based Overload Selection
```cpp
template <typename T>
void do_work(T value, std::true_type) { /* fast path */ }

template <typename T>
void do_work(T value, std::false_type) { /* slow path */ }

template <typename T>
void dispatch(T value) {
    do_work(value, std::is_integral<T>{});
}
```

## Testing Tag Dispatching

### 1. Compile-Time Selection
```cpp
static_assert(std::is_same_v<decltype(select_tag<int*>()), random_access_iterator_tag>);
static_assert(std::is_same_v<decltype(select_tag<std::vector<int>::iterator>()), input_iterator_tag>);
```

### 2. Output Verification
- Run the sample and verify output matches expected dispatch

## Advanced Usage Patterns

### 1. Tag Dispatching with Concepts (C++20+)
```cpp
template <typename T>
concept Fast = requires(T t) { t.fast(); };

template <Fast T>
void compute(T value) { /* fast path */ }

template <typename T>
void compute(T value) { /* slow path */ }
```

## Why Not Use enum class Instead of Tag Types?

While `enum class` is useful for representing a set of discrete values at runtime, tag dispatching with empty structs is preferred for compile-time algorithm selection in Modern C++ for several reasons:

- **Compile-Time Overload Resolution:** Tag types enable function overloads to be selected at compile time, allowing the compiler to optimize away any dispatch logic. `enum class` values are runtime objects and require switch/case or if/else logic, which incurs runtime overhead.
- **Type Safety and Extensibility:** Tag types can participate in inheritance hierarchies (e.g., `random_access_iterator_tag : input_iterator_tag`), enabling more flexible and extensible dispatch. `enum class` values are fixed and cannot express relationships between categories.
- **Zero Runtime Cost:** Tag dispatching incurs no runtime cost—there are no branches or switches, just direct calls to the correct overload. `enum class` dispatch always requires a runtime check.
- **Template Metaprogramming:** Tag types integrate seamlessly with template metaprogramming and SFINAE, allowing for more expressive and generic code.
- **STL Compatibility:** The C++ Standard Library uses tag types (not enums) for iterator categories and other compile-time traits, so following this pattern ensures compatibility and familiarity.

**Summary:**
- Use tag types for compile-time, type-based dispatch and algorithm selection.
- Use `enum class` for runtime state or when you need a variable to hold a value from a set of options.

Tag dispatching is a powerful, zero-overhead technique for customizing algorithms and selecting optimal implementations at compile time in Modern C++.
