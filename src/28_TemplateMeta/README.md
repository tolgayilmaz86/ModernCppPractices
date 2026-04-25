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
| C++17 | `if constexpr`, fold expressions, CTAD(Class Template Argument Deduction) |
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
// C++14
template <std::size_t... Is, typename Tuple>
void print_tuple_impl(std::index_sequence<Is...>, const Tuple& t) {
    ((std::cout << std::get<Is>(t) << " "), ...);
}

// C++17
template <typename... Ts>
void print_tuple(const std::tuple<Ts...>& t) {
    print_tuple_impl(std::index_sequence_for<Ts...>{}, t);
}

print_tuple(std::make_tuple(1, 2.5, 'c', "hello")); // 1 2.5 c hello
```

---

### 4. `if constexpr` (C++17)

`if constexpr` selects a branch **at compile time**. The discarded branch is not instantiated, so it can contain code that would be ill-formed for other types — eliminating the need for multiple overloads.

```cpp
// Old way:
template <typename T, typename = void>
struct SFINAE_describe : std::false_type {};
template <typename T>
struct SFINAE_describe<T, std::enable_if_t<std::is_integral_v<T>>> : std::true_type {};

// C++17
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
// describe(std::string("hello")) → "other"
```

**Before C++17:** you would need tag dispatch or explicit specialisations for every combination.

---

### 5. Fold Expressions (C++17)

Fold expressions collapse a parameter pack with a binary operator in one concise expression.

```cpp
// Left fold:  (((1 + 2) + 3) + 4)
template <typename... Args>
auto sum(Args... args) { return (... + args); }

sum(1, 2, 3, 4, 5);  // 15

// Right fold: (1 * (2 * (3 * 4)))
template <typename... Args>
auto product(Args... args) { return (args * ...); }

product(1, 2, 3, 4, 5);  // 120

// Binary fold with initial value
template <typename... Args>
auto sum_from(int init, Args... args) { return (init + ... + args); }

sum_from(1, 2, 3, 4, 5);  // 15

// Practical: push all elements at once
template <typename T, typename... Args>
void push_all(std::vector<T>& v, Args&&... args) {
    (v.push_back(std::forward<Args>(args)), ...);
}

std::vector<int> v;
push_all(v, 1, 2, 3, 4, 5);  // v now contains 1, 2, 3, 4, 5

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

Specialisation lets you provide a **type-specific** implementation while keeping a generic primary template. It is useful for:

- Optimizing for specific types
- Adding new capabilities to existing template
- Partial specialisation for categories of types

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
c.area(1.0); // 3.14159265358979
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

### 9. Macros vs. Templates — replacing the preprocessor

Preprocessor macros predate the template system and have well-known problems: no type safety, no scoping, no debugging, and subtle pitfalls with multiple evaluation. Modern C++ templates solve every one of these.

#### 9a. Object-like macros → `constexpr` / variable templates

```cpp
// BAD — no type, no scope, replaces text blindly
#define MAX_SIZE 1024
#define PI 3.14159265358979

// GOOD — typed, scoped, usable in debuggers
inline constexpr std::size_t MAX_SIZE = 1024;

template <typename T = double>          // precision chosen at call site
inline constexpr T PI_v = static_cast<T>(3.14159265358979323846L);

double area = PI_v<double> * r * r;     // full precision
float  area = PI_v<float>  * r * r;     // narrowed safely & explicitly
```

#### 9b. Function-like macros → `constexpr` / `inline` templates

The classic macro pitfall: `#define SQUARE(x) x*x` evaluates `x` **twice**, so `SQUARE(i++)` corrupts `i`.

```cpp
// BAD
#define SQUARE(x) ((x)*(x))     // SQUARE(i++) → i++ * i++  (UB!)
#define MAX(a,b)  ((a)>(b)?(a):(b))
#define ABS(x)    ((x)<0?-(x):(x))

// GOOD — evaluated once, type-safe, inlined by the compiler
template <typename T>
constexpr T tmpl_square(T x) noexcept { return x * x; }

template <typename T>
requires std::totally_ordered<T>
constexpr T tmpl_max(T a, T b) noexcept { return a > b ? a : b; }

template <typename T>
requires std::is_arithmetic_v<T>
constexpr T tmpl_abs(T x) noexcept { return x < T{} ? -x : x; }

int i = 4;
int r = tmpl_square(i++);   // i++ evaluated once: r=16, i=5 ✓
```

#### 9c. Logging macros → variadic template + `std::source_location` (C++20)

`__FILE__` and `__LINE__` are the main reason logging macros survived so long. C++20 renders them unnecessary.

```cpp
// BAD
#define LOG(fmt, ...) printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// GOOD — std::source_location captures call site as a real type
#include <source_location>

template <typename... Args>
void log(std::string_view msg,
         Args&&... args,
         const std::source_location loc = std::source_location::current())
{
    std::cout << '[' << loc.file_name() << ':' << loc.line() << "] "
              << msg;
    ((std::cout << ' ' << args), ...);
    std::cout << '\n';
}

log("starting");              // [tmpsample.cpp:42] starting
log("value:", 42, 3.14);      // [tmpsample.cpp:43] value: 42 3.14
```

#### 9d. `printf` → type-safe variadic template

```cpp
// BAD — runtime mismatch between format string and argument types
printf("%d %s", some_int, some_string.c_str());

// GOOD — all types resolved at compile time
template <typename T, typename... Rest>
void safe_printf_impl(std::ostream& out, std::string_view fmt,
                      const T& head, const Rest&... rest) {
    auto pos = fmt.find("{}");
    out << fmt.substr(0, pos) << head;
    safe_printf_impl(out, fmt.substr(pos + 2), rest...);
}

safe_printf("Hello {}! You are {} years old.", name, age);
// → Hello Alice! You are 30 years old.
// Passing wrong type? Caught at compile time, not at runtime.
```

> In C++20 use `std::format` / `std::print` instead — the same idea, standardised.

#### 9e. Boilerplate macros → CRTP mixins

Macros like `DECLARE_SINGLETON(T)` or `MAKE_COMPARABLE(T)` inject copy-pasted code. CRTP mixins inject behaviour through the type system, are debuggable, and compose naturally.

```cpp
// BAD
#define MAKE_COMPARABLE(T)        \
    bool operator!=(const T& o) const { return !(*this == o); } \
    bool operator> (const T& o) const { return o < *this; }    \
    bool operator<=(const T& o) const { return !(o < *this); } \
    bool operator>=(const T& o) const { return !(*this < o); }

// GOOD — one template mixin, reused by any type
template <typename Derived>
struct Comparable {
    bool operator!=(const Derived& o) const noexcept {
        return !static_cast<const Derived*>(this)->operator==(o); }
    bool operator> (const Derived& o) const noexcept {
        return o < *static_cast<const Derived*>(this); }
    bool operator<=(const Derived& o) const noexcept {
        return !(o < *static_cast<const Derived*>(this)); }
    bool operator>=(const Derived& o) const noexcept {
        return !(*static_cast<const Derived*>(this) < o); }
};

struct Temperature : Comparable<Temperature> {
    double celsius;
    bool operator==(const Temperature& o) const noexcept { return celsius == o.celsius; }
    bool operator< (const Temperature& o) const noexcept { return celsius <  o.celsius; }
};
// Temperature now has all six comparison operators — zero macro paste.
```

#### 9f. Compilation-flag macros → `if constexpr` policy template

`#ifdef`/`#endif` guards scatter conditional compilation across every call site and are invisible to the type system.

```cpp
// BAD — every call site must guard manually
#ifdef ENABLE_LOGGING
    log("value:", x);
#endif

// GOOD — zero-overhead when disabled, single point of control
template <bool Enabled>
struct Logger {
    template <typename... Args>
    static void log([[maybe_unused]] std::string_view msg,
                    [[maybe_unused]] Args&&... args) {
        if constexpr (Enabled) {          // branch eliminated when false
            std::cout << "[LOG] " << msg;
            ((std::cout << ' ' << args), ...);
            std::cout << '\n';
        }
    }
};

using DebugLogger   = Logger<true>;
using ReleaseLogger = Logger<false>;  // all log() calls compile to nothing

DebugLogger::log("processing", item);   // printed in debug builds
ReleaseLogger::log("processing", item); // completely elided
```

#### 9g. Assertion macros → `static_assert` + Concepts

```cpp
// BAD — C++03 hack
#define STATIC_ASSERT(cond)  typedef char _static_assert_[(cond) ? 1 : -1]

// GOOD — built-in, gives a human-readable message
static_assert(sizeof(int) == 4, "This code requires 32-bit int");

// BETTER — reusable template constraint
template <typename T>
constexpr void require_trivially_copyable() {
    static_assert(std::is_trivially_copyable_v<T>,
                  "T must be trivially copyable for use in this buffer");
}

require_trivially_copyable<int>();      // OK
require_trivially_copyable<std::string>(); // compile error with clear message

// BEST — C++20 Concept as a constraint, not just an assertion
template <typename T>
requires std::is_trivially_copyable_v<T>
class FastBuffer { /* ... */ };
```

#### Summary — macro-to-template migration table

| Macro pattern | C++ template replacement | Benefit |
|---------------|--------------------------|---------|
| `#define CONST value` | `inline constexpr T name` | Typed, scoped, debuggable |
| `#define FN(x) expr` | `template<T> constexpr T fn(T x)` | Single evaluation, type-safe |
| `#define MAX(a,b) ...` | `std::max` / `tmpl_max<T>` | No double-eval pitfall |
| `#define LOG(fmt,...)` | variadic template + `source_location` | Scoped, typed, inspectable |
| `printf("%d", v)` | `safe_printf("{}", v)` / `std::print` | Compile-time type check |
| `#define BOILERPLATE(T)` | CRTP mixin template | Reusable, composable |
| `#ifdef FLAG / #endif` | `if constexpr` / `Logger<bool>` | Zero overhead, single site |
| `STATIC_ASSERT` hack | `static_assert` / Concepts | Clear messages, composable |

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

=== Macros vs. Templates ===

--- Constants ---
MAX_SIZE           = 1024
PI_v<double>       = 3.14159
PI_v<float>        = 3.14159

--- Function-like macros ---
tmpl_square(5)     = 25
tmpl_square(3.14)  = 9.8596
tmpl_max(3,7)      = 7
tmpl_abs(-42.5)    = 42.5
tmpl_square(x++) with x=4: result=16, x after=5 (x++ evaluated once)

--- Logging with source_location ---
[TMPSample.cpp:600] TMPSample demo running
[TMPSample.cpp:601] values: (42 3.14 hello )

--- Type-safe printf ---
Hello, World! You are 30 years old.
1 + 2 = 3

--- Boilerplate via CRTP ---
t1(20) < t2(37): 1
t1(20) > t2(37): 0
t1(20) == t3(20): 1
t1(20) != t2(37): 1
t2(37) >= t1(20): 1

--- Compilation-flag policy (if constexpr Logger) ---
[LOG] debug build message extra= 99
ReleaseLogger::log() emits no output (and no instructions)

--- static_assert as ASSERT macro ---
int and double pass trivially-copyable check

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
| Replacing `#define` constants | `inline constexpr` / variable templates |
| Replacing function-like macros | `constexpr` / `inline` template functions |
| Replacing logging macros | Variadic template + `std::source_location` |
| Replacing `#ifdef` guards | `if constexpr` / policy template `<bool>` |
| Replacing boilerplate macros | CRTP mixin templates |

### Performance

- All TMP is **zero-cost at runtime** — computation happens at compile time.
- Heavy template use can **increase compile times**; prefer `constexpr` functions over deep struct recursion where possible.
- Concepts produce better diagnostics and can speed up overload resolution compared to SFINAE.

### Common Pitfalls

1. **Deep recursion limits** — compilers impose a maximum template instantiation depth (typically 900–1024). Use iterative `constexpr` for large values.
2. **Cryptic SFINAE errors** — switch to Concepts where possible (C++20).
3. **Code bloat** — each unique template instantiation generates code; use explicit instantiation or `extern template` for common cases.
4. **Over-engineering** — TMP is powerful but adds complexity. Prefer simple runtime solutions unless compile-time behaviour is genuinely needed.
5. **Macro double-evaluation** — always replace function-like macros with templates; `#define SQUARE(x) x*x` with `x++` is undefined behaviour.
6. **`#ifdef` drift** — conditional compilation blocks are invisible to the type system and hard to test; prefer `if constexpr` or policy templates.

---

## Best Practices

1. **Prefer `constexpr` functions** over recursive struct templates for value computation.
2. **Prefer Concepts** (C++20) over raw SFINAE for template constraints.
3. **Use `if constexpr`** instead of multiple overloads for type-conditional behaviour.
4. **Wrap traits in `_v`/`_t` aliases** (`is_foo_v`, `remove_foo_t`) for cleaner call sites.
5. **Keep meta-programs simple** — if a meta-function is hard to read, refactor it.
6. **Eliminate function-like macros** — replace every `#define FN(x)` with a typed `constexpr` / `inline` template.
7. **Replace `#ifdef` blocks** with `if constexpr` or a `Logger<bool>`-style policy template.
8. **Use `std::source_location`** (C++20) instead of `__FILE__`/`__LINE__` macros in logging helpers.
9. **Use CRTP mixins** instead of `#define BOILERPLATE(T)` patterns for injecting repeated operator/interface code.

---

## Related Topics

- **[SFINAE](../02_SFINAE/README.md):** The substitution-failure mechanism that TMP relies on
- **[Concepts](../16_Concepts/README.md):** C++20 first-class constraints (the modern replacement for SFINAE)
- **[CRTP](../03_CRTP/README.md):** Curiously Recurring Template Pattern — TMP applied to static polymorphism
- **[Type Erasure](../06_TypeErasure/README.md):** Hiding type information at runtime
- **[Variant & Visitor](../07_VariantVisitor/README.md):** Type-safe sum types as TMP-free alternative
