# Template Meta Programming (TMP)

## Overview

**Template Meta Programming (TMP)** is a technique that uses C++ templates to perform computation at **compile time** rather than at runtime. The compiler itself becomes the "interpreter", executing template instantiations as programs and producing results that are baked directly into the binary.

**The Problem:** Some computations — type dispatching, size calculations, algorithm selection, constraint checking — are known at compile time but traditionally forced into runtime code, adding unnecessary overhead and delaying error detection.

**The Solution:** TMP lets you express these computations entirely in the type system, generating zero-overhead code with errors caught at compile time.

---

## How TMP Works

```
Source code  →  Template instantiation  →  Compiler "runs" the meta-program
                      (compile time)
                          ↓
               Optimised machine code
               (zero TMP overhead at runtime)
```

There are three generations of TMP idioms in C++:

| Era | Key Tools |
|-----|-----------|
| C++98/03 | Recursive struct templates, full/partial specialisation |
| C++11/14 | `constexpr`, variadic templates, `type_traits`, variable templates |
| C++17 | `if constexpr`, fold expressions, CTAD |
| C++20 | Concepts, `consteval`, relaxed NTTPs, template lambdas |

---

## Techniques Covered

### 1. Compile-Time Values

The oldest TMP pattern: encode values as `static constexpr` members of a struct, and use **recursive specialisation** to perform computation.

```cpp
// Classic: compile-time factorial
template <unsigned N>
struct Factorial {
    static constexpr unsigned value = N * Factorial<N - 1>::value;
};
template <>
struct Factorial<0> {
    static constexpr unsigned value = 1;
};

static_assert(Factorial<5>::value == 120);  // checked at compile time

// Modern alternative: constexpr function (C++14+)
constexpr unsigned factorial(unsigned n) {
    return n == 0 ? 1 : n * factorial(n - 1);
}
static_assert(factorial(5) == 120);
```

> **Prefer `constexpr` functions** over recursive structs in modern C++. They are easier to read, debug, and often compile faster. Recursive structs remain useful when the result must be a **type**, not just a value.

---

### 2. Type Traits

Type traits are compile-time predicates and transformations on types. The standard library (`<type_traits>`) ships dozens; you can also write your own.

```cpp
// Query traits
static_assert(std::is_integral_v<int>);
static_assert(!std::is_integral_v<double>);
static_assert(std::is_pointer_v<int*>);

// Transform types
using T = std::remove_const_t<const int>;   // int
using U = std::add_pointer_t<double>;        // double*

// Custom trait: detect raw pointers
template <typename T> struct is_raw_pointer       : std::false_type {};
template <typename T> struct is_raw_pointer<T*>   : std::true_type  {};

// C++17 variable template helper
template <typename T>
inline constexpr bool is_raw_pointer_v = is_raw_pointer<T>::value;

static_assert( is_raw_pointer_v<int*>);
static_assert(!is_raw_pointer_v<int> );
```

**Common standard traits:**

| Trait | Purpose |
|-------|---------|
| `std::is_integral<T>` | Is `T` an integer type? |
| `std::is_floating_point<T>` | Is `T` a float/double/long double? |
| `std::is_same<T, U>` | Are `T` and `U` identical? |
| `std::is_base_of<B, D>` | Does `D` derive from `B`? |
| `std::remove_const_t<T>` | Strip top-level `const` |
| `std::decay_t<T>` | Array-to-pointer, function-to-pointer, remove cv-ref |
| `std::conditional_t<B, T, F>` | `T` if `B` else `F` |
| `std::enable_if_t<B, T>` | `T` if `B` else substitution failure |

---

### 3. Variadic Templates

Variadic templates accept an **arbitrary number** of template arguments, enabling truly generic containers and algorithms.

```cpp
// Recursive variadic sum
template <typename T>
T variadic_sum(T v) { return v; }                    // base case

template <typename T, typename... Rest>
T variadic_sum(T first, Rest... rest) {
    return first + variadic_sum(static_cast<T>(rest)...);
}

variadic_sum(1, 2, 3, 4, 5);   // 15

// Parameter pack size
template <typename... Ts>
constexpr std::size_t pack_size() { return sizeof...(Ts); }

static_assert(pack_size<int, double, char>() == 3);
```

**Index sequences** unlock tuple access:

```cpp
template <std::size_t... Is, typename Tuple>
void print_tuple_impl(std::index_sequence<Is...>, const Tuple& t) {
    ((std::cout << std::get<Is>(t) << " "), ...);
}

template <typename... Ts>
void print_tuple(const std::tuple<Ts...>& t) {
    print_tuple_impl(std::index_sequence_for<Ts...>{}, t);
}
```

---

### 4. `if constexpr` (C++17)

`if constexpr` selects a branch **at compile time**. The discarded branch is not instantiated, so it can contain code that would be ill-formed for other types — eliminating the need for multiple overloads.

```cpp
template <typename T>
std::string describe(const T& v) {
    if constexpr (std::is_integral_v<T>) {
        return "integer: " + std::to_string(v);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "float: "   + std::to_string(v);
    } else {
        return "other";
    }
}
// describe(42)   → "integer: 42"
// describe(3.14) → "float: 3.140000"
```

**Before C++17:** you would need tag dispatch or explicit specialisations for every combination.

---

### 5. Fold Expressions (C++17)

Fold expressions collapse a parameter pack with a binary operator in one concise expression.

```cpp
// Left fold:  (((1 + 2) + 3) + 4)
template <typename... Args>
auto sum(Args... args) { return (... + args); }

// Right fold: (1 * (2 * (3 * 4)))
template <typename... Args>
auto product(Args... args) { return (args * ...); }

// Binary fold with initial value
template <typename... Args>
auto sum_from(int init, Args... args) { return (init + ... + args); }

// Practical: push all elements at once
template <typename T, typename... Args>
void push_all(std::vector<T>& v, Args&&... args) {
    (v.push_back(std::forward<Args>(args)), ...);
}
```

**Four fold forms:**

| Form | Expansion |
|------|-----------|
| `(... op pack)` | `((a op b) op c) op ...` — unary left fold |
| `(pack op ...)` | `a op (b op (c op ...))` — unary right fold |
| `(init op ... op pack)` | `((init op a) op b) op ...` — binary left fold |
| `(pack op ... op init)` | `a op (b op (... op init))` — binary right fold |

---

### 6. Template Specialisation

Specialisation lets you provide a **type-specific** implementation while keeping a generic primary template.

```cpp
// Primary template
template <typename T>
struct TypeDescriptor { static std::string describe() { return "generic"; } };

// Full specialisation for int
template <>
struct TypeDescriptor<int> { static std::string describe() { return "int32"; } };

// Partial specialisation for any pointer
template <typename T>
struct TypeDescriptor<T*> {
    static std::string describe() {
        return "pointer to " + TypeDescriptor<T>::describe();
    }
};

TypeDescriptor<int>::describe();   // "int32"
TypeDescriptor<int*>::describe();  // "pointer to int32"
TypeDescriptor<int**>::describe(); // "pointer to pointer to int32"
```

**Policy-based design** is a powerful TMP pattern that parameterises behaviour through template arguments rather than virtual functions:

```cpp
template <typename SortPolicy>
class SortedBuffer {
    std::vector<int> data_;
    SortPolicy       cmp_;
public:
    void insert(int v) {
        auto pos = std::lower_bound(data_.begin(), data_.end(), v, cmp_);
        data_.insert(pos, v);
    }
};

SortedBuffer<std::less<int>>    ascending;
SortedBuffer<std::greater<int>> descending;
```

---

### 7. SFINAE vs. Concepts

#### SFINAE (C++11/14/17)

**Substitution Failure Is Not An Error.** When template argument substitution fails, the compiler silently drops that candidate instead of raising an error.

```cpp
// Enable only for arithmetic types
template <typename T,
          typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T square(T x) { return x * x; }

square(5);     // OK — int is arithmetic
square("hi");  // Compile error — std::string is not arithmetic
```

SFINAE works but leads to verbose, hard-to-read error messages.

#### Concepts (C++20)

Concepts are **named predicates** on template arguments, with clean error messages and first-class syntax.

```cpp
// Named concept
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Using the concept
template <Numeric T>
T square(T x) { return x * x; }

// Abbreviated syntax
auto square(Numeric auto x) { return x * x; }

// Inline requires clause
template <typename T>
requires std::integral<T>
T only_ints(T x) { return x; }

// Custom concept
template <typename T>
concept Printable = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream&>;
};
```

**Comparison:**

| Feature | SFINAE | Concepts (C++20) |
|---------|--------|-----------------|
| Syntax | Verbose (`enable_if_t`) | Clean, readable |
| Error messages | Cryptic template walls | Precise and helpful |
| Composability | Clumsy | Natural (`&&`, `\|\|`) |
| Requires clause | No | Yes |
| Subsumption | No | Yes (partial ordering) |

---

### 8. C++20 TMP Features

#### `consteval` — guaranteed compile-time evaluation

```cpp
consteval int square(int n) { return n * n; }

constexpr int v = square(7);   // OK: compile time
// int x = square(runtime_val); // Error: consteval must run at compile time
```

#### Non-Type Template Parameters (NTTP) with floating-point (C++20)

C++20 lifts the restriction that NTTPs must be integral. Any literal type can be a NTTP:

```cpp
template <double Pi>
struct Circle {
    double area(double r) { return Pi * r * r; }
};

Circle<3.14159265358979> c;
```

#### Template Lambdas (C++20)

```cpp
auto add = []<typename T>(T a, T b) -> T { return a + b; };

add(1, 2);       // T = int
add(1.5, 2.5);   // T = double
```

#### `std::is_constant_evaluated()` (C++20)

Lets a single `constexpr` function take different code paths depending on whether it is evaluated at compile or runtime:

```cpp
constexpr int factorial(int n) {
    if (std::is_constant_evaluated()) {
        // compile-time path (can use only constexpr operations)
        int r = 1;
        for (int i = 1; i <= n; ++i) r *= i;
        return r;
    } else {
        return lookup_table[n]; // runtime path
    }
}
```

---

## Sample Output

```
Running Template Meta Programming (TMP) Sample...

=== Compile-Time Values ===
Factorial<5>::value  = 120
Factorial<10>::value = 3628800
Fibonacci<10>::value = 55
Power<2,10>::value   = 1024
constexpr_factorial(7) = 5040
All compile-time assertions passed!

=== Type Traits ===
std::is_integral<int>::value       : 1
std::is_integral<double>::value    : 0
std::is_pointer<int*>::value       : 1
remove_const<const int>   -> int   : 1
is_raw_pointer_v<int>    : 0
is_raw_pointer_v<int*>   : 1
array_rank<int[2][4]>    : 2

=== Variadic Templates ===
variadic_sum(1,2,3,4,5)     = 15
print_all(42, 3.14, "hello", true): 42, 3.14, hello, 1

=== if constexpr (C++17) ===
describe_type(42)       : integral(42)
describe_type(3.14)     : float(3.140000)

=== Fold Expressions (C++17) ===
fold_sum(1,2,3,4,5)         = 15
fold_product(1,2,3,4,5)     = 120
fold_all_positive(1,2,3)    = 1
fold_all_positive(1,-1,3)   = 0

=== Template Specialization ===
TypeDescriptor<int>::describe()      : 32-bit signed integer
TypeDescriptor<int*>::describe()     : pointer to 32-bit signed integer
TypeDescriptor<int**>::describe()    : pointer to pointer to 32-bit signed integer

=== SFINAE vs. Concepts ===
SFINAE sfinae_square(5)    = 25
Concept concept_square(7)   = 49

=== C++20 TMP Features ===
consteval ce_square(7) = 49
CompileTimeDouble<3.14>::value = 3.14
gen_adder<int>(3,4)     = 7

TMP demonstration completed!
```

---

## Key Concepts Summary

### When to Use TMP

| Scenario | Recommended Technique |
|----------|----------------------|
| Compile-time constant | `constexpr` / `consteval` |
| Type introspection | `std::type_traits` / custom traits |
| Constrain templates | Concepts (C++20) / `enable_if` (C++11) |
| N-ary generic functions | Variadic templates + fold expressions |
| Compile-time branching | `if constexpr` |
| Behaviour policy injection | Policy-based design (template parameters) |
| Type-specific overrides | Template specialisation |

### Performance

- All TMP is **zero-cost at runtime** — computation happens at compile time.
- Heavy template use can **increase compile times**; prefer `constexpr` functions over deep struct recursion where possible.
- Concepts produce better diagnostics and can speed up overload resolution compared to SFINAE.

### Common Pitfalls

1. **Deep recursion limits** — compilers impose a maximum template instantiation depth (typically 900–1024). Use iterative `constexpr` for large values.
2. **Cryptic SFINAE errors** — switch to Concepts where possible (C++20).
3. **Code bloat** — each unique template instantiation generates code; use explicit instantiation or `extern template` for common cases.
4. **Over-engineering** — TMP is powerful but adds complexity. Prefer simple runtime solutions unless compile-time behaviour is genuinely needed.

---

## Best Practices

1. **Prefer `constexpr` functions** over recursive struct templates for value computation.
2. **Prefer Concepts** (C++20) over raw SFINAE for template constraints.
3. **Use `if constexpr`** instead of multiple overloads for type-conditional behaviour.
4. **Wrap traits in `_v`/`_t` aliases** (`is_foo_v`, `remove_foo_t`) for cleaner call sites.
5. **Keep meta-programs simple** — if a meta-function is hard to read, refactor it.

---

## Related Topics

- **[SFINAE](../02_SFINAE/README.md):** The substitution-failure mechanism that TMP relies on
- **[Concepts](../16_Concepts/README.md):** C++20 first-class constraints (the modern replacement for SFINAE)
- **[CRTP](../03_CRTP/README.md):** Curiously Recurring Template Pattern — TMP applied to static polymorphism
- **[Type Erasure](../06_TypeErasure/README.md):** Hiding type information at runtime
- **[Variant & Visitor](../07_VariantVisitor/README.md):** Type-safe sum types as TMP-free alternative
