# C++20 Concepts

## Overview

**Concepts** are a C++20 feature that provides a clean, readable way to constrain template parameters. They are the modern replacement for SFINAE (Substitution Failure Is Not An Error), offering better syntax and dramatically improved error messages.

## The Problem: SFINAE is Hard to Read

### ❌ Traditional SFINAE Approach
```cpp
// Type trait to detect if type has size() method
template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> 
    : std::true_type {};

// Constrained function using enable_if
template <typename T>
typename std::enable_if<has_size<T>::value, std::size_t>::type
get_size(const T& container) {
    return container.size();
}

template <typename T>
typename std::enable_if<!has_size<T>::value, std::size_t>::type
get_size(const T&) {
    return 1;
}
```

**Problems:**
- Verbose and hard to read
- Complex nested template syntax
- Cryptic error messages
- Intent is obscured by implementation details

### ✅ C++20 Concepts Approach
```cpp
// Clean concept definition
template <typename T>
concept Sizable = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

// Constrained function using concept
std::size_t get_size(const Sizable auto& container) {
    return container.size();
}

std::size_t get_size(const auto&) {
    return 1;  // Fallback
}
```

**Benefits:**
- Readable and self-documenting
- Clear intent
- Excellent error messages
- Easier to maintain

## Concept Syntax

### Defining Concepts

```cpp
// Using type traits
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// Using requires expression
template <typename T>
concept Container = requires(T t) {
    typename T::value_type;           // Type requirement
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};

// Composing concepts
template <typename T>
concept SortableContainer = Container<T> && 
    std::totally_ordered<typename T::value_type>;
```

### Using Concepts

```cpp
// Method 1: Template parameter constraint
template <Numeric T>
T double_value(T value) { return value * 2; }

// Method 2: Trailing requires clause
template <typename T>
    requires Numeric<T>
T triple_value(T value) { return value * 3; }

// Method 3: Abbreviated function template
void print(const Printable auto& value) {
    std::cout << value << std::endl;
}

// Method 4: Constrained auto return
Numeric auto compute(Numeric auto a, Numeric auto b) {
    return a + b;
}
```

## Requires Expressions

```cpp
template <typename T>
concept MyConstraint = requires(T t, T other) {
    // Simple requirement: expression must compile
    t.foo();
    
    // Type requirement: type must exist
    typename T::value_type;
    
    // Compound requirement: expression + return type
    { t.bar() } -> std::convertible_to<int>;
    { t + other } -> std::same_as<T>;
    
    // Nested requirement: additional predicate
    requires sizeof(T) <= 16;
    requires std::is_copy_constructible_v<T>;
};
```

## Standard Library Concepts

| Concept | Header | Description |
|---------|--------|-------------|
| `std::integral` | `<concepts>` | Integer types |
| `std::floating_point` | `<concepts>` | Float/double types |
| `std::signed_integral` | `<concepts>` | Signed integers |
| `std::unsigned_integral` | `<concepts>` | Unsigned integers |
| `std::same_as<T, U>` | `<concepts>` | Types are identical |
| `std::convertible_to<From, To>` | `<concepts>` | Implicit conversion exists |
| `std::derived_from<D, B>` | `<concepts>` | Inheritance relationship |
| `std::totally_ordered` | `<concepts>` | Supports <, >, <=, >=, ==, != |
| `std::copyable` | `<concepts>` | Copy constructible and assignable |
| `std::movable` | `<concepts>` | Move constructible and assignable |
| `std::regular` | `<concepts>` | Default constructible + copyable + equality |
| `std::invocable<F, Args...>` | `<concepts>` | Callable with given arguments |
| `std::predicate<F, Args...>` | `<concepts>` | Returns bool-convertible |
| `std::ranges::range` | `<ranges>` | Has begin() and end() |
| `std::input_iterator` | `<iterator>` | Input iterator requirements |
| `std::random_access_iterator` | `<iterator>` | Random access iterator |

## Concept Subsumption (Overload Resolution)

When multiple overloads match, the **more constrained** overload wins:

```cpp
template <typename T>
concept Addable = requires(T a, T b) { a + b; };

template <typename T>
concept Numeric = Addable<T> && std::is_arithmetic_v<T>;

// Less constrained
template <Addable T>
T process(T a, T b) { return a + b; }

// More constrained - preferred when both match
template <Numeric T>
T process(T a, T b) { return a + b; }

process(1, 2);           // Calls Numeric version
process(str1, str2);     // Calls Addable version (strings not Numeric)
```

## Concepts with Classes

```cpp
// Constrained class template
template <std::integral T>
class Counter {
    T value_;
public:
    void increment() { ++value_; }
    T get() const { return value_; }
};

// Constrained member functions
class Container {
public:
    template <std::random_access_iterator Iter>
    void fast_assign(Iter begin, Iter end);
    
    template <std::input_iterator Iter>
        requires (!std::random_access_iterator<Iter>)
    void slow_assign(Iter begin, Iter end);
};
```

## Error Message Comparison

### SFINAE Error (cryptic)
```
error: no matching function for call to 'process(std::string)'
note: candidate template ignored: substitution failure
      [with T = std::string]: no type named 'type' in 
      'struct std::enable_if<false, void>'
```

### Concepts Error (clear)
```
error: no matching function for call to 'process'
note: constraints not satisfied
note: because 'std::string' does not satisfy 'Numeric'
note: because 'std::is_arithmetic_v<std::string>' evaluated to false
```

## When to Use

| Use Concepts When | Stick with SFINAE When |
|-------------------|------------------------|
| C++20 or later available | Pre-C++20 codebase |
| Template constraints needed | Simple `enable_if` suffices |
| API clarity is important | Internal implementation detail |
| Better errors desired | Compatibility required |

## Key Benefits

1. **Readability**: Intent is clear from the code
2. **Maintainability**: Easier to modify and extend
3. **Error Messages**: Compiler tells you exactly what failed
4. **Composition**: Combine concepts with `&&` and `||`
5. **Subsumption**: Automatic overload preference
6. **Self-Documentation**: Constraints serve as documentation

## Evolution Path

```
C++98: Function overloading
  ↓
C++11: SFINAE with enable_if
  ↓
C++17: if constexpr for compile-time branching
  ↓
C++20: Concepts for clean constraints ← Modern approach
```

## See Also

- [02_SFINAE](../02_SFINAE/README.md) - The traditional approach concepts replace
- [11_TagDispatching](../11_TagDispatching/README.md) - Another technique for compile-time dispatch
