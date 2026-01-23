# C++20 Three-Way Comparison (Spaceship Operator)

## Overview

The **three-way comparison operator** (`<=>`), also known as the **spaceship operator**, was introduced in C++20 to simplify comparison operations. By defining just `<=>` (and optionally `==`), the compiler automatically generates all six comparison operators: `==`, `!=`, `<`, `>`, `<=`, `>=`.

## The Problem Before C++20

Before C++20, implementing full comparison support required writing up to 6 operators:

```cpp
// Pre-C++20: Tedious and error-prone
class Version {
    int major, minor, patch;
public:
    bool operator==(const Version& o) const { 
        return major == o.major && minor == o.minor && patch == o.patch; 
    }
    bool operator!=(const Version& o) const { return !(*this == o); }
    bool operator<(const Version& o) const { 
        return std::tie(major, minor, patch) < std::tie(o.major, o.minor, o.patch); 
    }
    bool operator>(const Version& o) const { return o < *this; }
    bool operator<=(const Version& o) const { return !(o < *this); }
    bool operator>=(const Version& o) const { return !(*this < o); }
};
```

## The C++20 Solution

```cpp
// C++20: Just one operator!
class Version {
    int major, minor, patch;
public:
    auto operator<=>(const Version&) const = default;
};
// Compiler generates: ==, !=, <, >, <=, >= automatically!
```

## Ordering Categories

The return type of `<=>` determines the **ordering category** and which operators are generated:

### 1. `std::strong_ordering`

- **Values:** `less`, `equal`, `greater`
- **Meaning:** `equal` means values are **substitutable** (identical for all purposes)
- **Generates:** `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Use when:** Values are totally ordered and equality means identity

```cpp
class Person {
    std::string name;
    int age;
public:
    // Defaulted: compares members in declaration order
    auto operator<=>(const Person&) const = default;
    bool operator==(const Person&) const = default;
};
```

### 2. `std::weak_ordering`

- **Values:** `less`, `equivalent`, `greater`
- **Meaning:** `equivalent` means same for comparison but may differ otherwise
- **Generates:** `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Use when:** Objects can be equivalent without being identical

```cpp
class CaseInsensitiveString {
    std::string str;
public:
    std::weak_ordering operator<=>(const CaseInsensitiveString& other) const {
        // "Hello" and "HELLO" are equivalent but not identical
        // ... case-insensitive comparison ...
        return std::weak_ordering::equivalent;
    }
    bool operator==(const CaseInsensitiveString& o) const {
        return (*this <=> o) == 0;
    }
};
```

### 3. `std::partial_ordering`

- **Values:** `less`, `equivalent`, `greater`, `unordered`
- **Meaning:** Some values cannot be compared at all
- **Generates:** `==`, `!=`, `<`, `>`
- **Use when:** Not all values are comparable (e.g., `NaN` in floating-point)

```cpp
class OptionalValue {
    bool has_value;
    double value;
public:
    std::partial_ordering operator<=>(const OptionalValue& other) const {
        if (!has_value || !other.has_value)
            return std::partial_ordering::unordered;
        if (value < other.value) return std::partial_ordering::less;
        if (value > other.value) return std::partial_ordering::greater;
        return std::partial_ordering::equivalent;
    }
};
```

## Key Rules

### Rule 1: Defaulted `<=>` implies defaulted `==`

```cpp
class Point {
    int x, y;
public:
    auto operator<=>(const Point&) const = default;
    // == is also automatically defaulted!
};
```

### Rule 2: Custom `<=>` requires explicit `==`

```cpp
class MyClass {
public:
    std::strong_ordering operator<=>(const MyClass& o) const {
        // custom logic
    }
    // MUST define == explicitly when <=> is custom!
    bool operator==(const MyClass& o) const {
        // custom equality logic
    }
};
```

### Rule 3: Defining `==` also defines `!=`

```cpp
class MyClass {
public:
    bool operator==(const MyClass&) const = default;
    // != is automatically generated!
};
```

### Rule 4: Ordering categories form a hierarchy

```
strong_ordering → weak_ordering → partial_ordering
     (most restrictive)      (least restrictive)
```

Implicit conversions go from stronger to weaker:
```cpp
std::strong_ordering s = std::strong_ordering::less;
std::weak_ordering w = s;     // OK: implicit conversion
std::partial_ordering p = w;  // OK: implicit conversion
```

## Comparison Table

| Category | Values | `equal` vs `equivalent` | Generated Operators |
|----------|--------|-------------------------|---------------------|
| `strong_ordering` | `less`, `equal`, `greater` | `equal` = substitutable | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| `weak_ordering` | `less`, `equivalent`, `greater` | `equivalent` = same order, different identity | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| `partial_ordering` | `less`, `equivalent`, `greater`, `unordered` | Some pairs incomparable | `==`, `!=`, `<`, `>` |

## Using `<=>` Directly

You can use the spaceship operator directly to get a three-way comparison result:

```cpp
Version v1{1, 0, 0};
Version v2{2, 0, 0};

auto result = v1 <=> v2;

if (result < 0)      std::cout << "v1 < v2";
else if (result > 0) std::cout << "v1 > v2";
else                 std::cout << "v1 == v2";

// Or check specific values:
if (result == std::strong_ordering::less) {
    std::cout << "v1 is less than v2";
}
```

## Expected Output

```
=== Strong Ordering: Person (defaulted <=>) ===
Alice vs Bob:
  alice < bob:  false
  alice > bob:  true
  alice == bob: false
  alice != bob: true

Alice vs Alice2 (same values):
  alice == alice2: true
  alice <= alice2: true
  alice >= alice2: true

Sorted people: Alice(20) Alice(30) Bob(25) Charlie(35) 

=== Strong Ordering: Version (custom <=>) ===
v1 = 1.0.0, v2 = 1.2.0, v3 = 1.2.3, v4 = 2.0.0
v1 < v2:  true
v2 < v3:  true
v3 < v4:  true
v1 == v1: true
1.0.0 is less than 2.0.0

=== Weak Ordering: CaseInsensitiveString ===
Comparing "Hello" and "HELLO":
  s1 == s2 (equivalent): true
  s1 < s2:  false
  s1 <= s2: true

=== Partial Ordering: OptionalValue (with NaN) ===
Comparing normal values (5.0 vs 10.0):
  a < b:  true
  a > b:  false
  a == b: false

Comparing with empty value (unordered):
  a < empty:  false
  a > empty:  false
  a == empty: false

Comparing with NaN (unordered):
  a < nan:  false
  a > nan:  false
  a == nan: false
  nan == nan: false
```

## Best Practices

1. **Prefer defaulted `<=>`** when member-wise comparison is correct
2. **Use `strong_ordering`** for types where equal means identical
3. **Use `weak_ordering`** for equivalence classes (case-insensitive strings, etc.)
4. **Use `partial_ordering`** only when values can be incomparable
5. **Always define `==`** when defining custom `<=>`
6. **Consider const correctness** - comparison operators should be `const`

## Related Features

- [16_Concepts](../16_Concepts/README.md) - Constraining types that support comparison
- [10_MoveSemantics](../10_MoveSemantics/README.md) - Efficient member comparison
- [05_RuleOfFive](../05_RuleOfFive/README.md) - Special member functions

## References

- [P0515R3 - Consistent comparison](https://wg21.link/p0515r3)
- [cppreference: three-way comparison](https://en.cppreference.com/w/cpp/language/operator_comparison#Three-way_comparison)
- [cppreference: comparison categories](https://en.cppreference.com/w/cpp/utility/compare)
