# C++ Cast Types

## Overview

C++ provides several cast operators for different types of type conversions. Choosing the right cast is crucial for safety, performance, and code clarity. This sample demonstrates all C++ cast types and their proper usage in modern C++.

**The Problem:** C-style casts like `(int)x` are unsafe and hide potential bugs. C++ provides specific cast operators for different scenarios.

**The Solution:** Use the appropriate cast operator for each situation - `static_cast`, `dynamic_cast`, `const_cast`, `reinterpret_cast`, or `std::bit_cast`.

## How C++ Casts Work

1. **static_cast:** Compile-time checked conversions between related types
2. **dynamic_cast:** Runtime-checked conversions for polymorphic types
3. **const_cast:** Add or remove const/volatile qualifiers
4. **reinterpret_cast:** Bit reinterpretation (dangerous)
5. **std::bit_cast:** Safe bit reinterpretation (C++20)

```cpp
// Safe conversions
double d = 3.14;
int i = static_cast<int>(d);  // OK: numeric conversion

// Runtime-checked polymorphic conversion
Animal* animal = new Dog();
if (Dog* dog = dynamic_cast<Dog*>(animal)) {
    // Safe: we know it's a Dog
}

// Dangerous: only for low-level code
float f = 3.14f;
uint32_t bits = std::bit_cast<uint32_t>(f);  // C++20: safe bit copy
```

## Sample Output

```
Running C++ Cast Types Sample...

=== static_cast - Safe Compile-Time Cast ===
double 3.14159 -> int 3 (truncated)
Dog* -> Animal*: Woof!
Animal* -> Dog*: Dog fetches ball
void* -> Dog*: Woof!

=== dynamic_cast - Runtime-Checked Cast ===
Animal says: Woof!
It's a Dog! Dog fetches ball
Animal says: Meow!
It's a Cat! Cat scratches furniture
Animal says: Woof!
It's a Dog! Dog fetches ball
Reference cast successful: Dog fetches ball

=== const_cast - Adding/Removing const ===
Original const string: Hello, World!
const_cast successful - but don't modify const data!
Added const: Mutable

=== reinterpret_cast - Bit Reinterpretation ===
float 3.14 as uint32_t: 1078523331
uintptr_t deadbeef -> int*: 00000000DEADBEEF
WARNING: reinterpret_cast is very dangerous!

=== std::bit_cast (C++20) - Safe Bit Reinterpretation ===
float 1 -> uint32_t: 1065353216
uint32_t 1065353216 -> float: 1
byte array -> float: 1

=== Casting Best Practices ===
1. Prefer static_cast for most conversions
2. Use dynamic_cast only with polymorphic types
3. Avoid const_cast when possible
4. Use reinterpret_cast only for low-level code
5. Prefer std::bit_cast over reinterpret_cast for bit manipulation
6. Avoid C-style casts: (int)x - use static_cast<int>(x)
7. Document why you're casting and why it's safe

=== Modern C++ Casting Guidelines ===
Smart pointers handle inheritance automatically: Woof!
Safe downcast successful: Dog fetches ball

=== Cast Types Summary ===
static_cast: Safe, compile-time checked conversions
dynamic_cast: Runtime-checked polymorphic conversions
const_cast: Add/remove const (dangerous)
reinterpret_cast: Bit reinterpretation (very dangerous)
std::bit_cast: Safe bit reinterpretation (C++20)

Choose the right cast for the job - wrong choice = bugs!

C++ Cast Types demonstration completed!
```

## Key Components

### 1. static_cast
- **Purpose:** Safe conversions that can be verified at compile time
- **Use cases:** Numeric conversions, inheritance hierarchy navigation, void* conversions
- **Safety:** Compile-time checked, generally safe

### 2. dynamic_cast
- **Purpose:** Runtime-checked conversions for polymorphic types
- **Use cases:** Safe downcasting in inheritance hierarchies
- **Safety:** Runtime checked, returns nullptr or throws std::bad_cast

### 3. const_cast
- **Purpose:** Add or remove const/volatile qualifiers
- **Use cases:** Working with legacy APIs that don't respect const-correctness
- **Safety:** Dangerous if used to modify truly const data

### 4. reinterpret_cast
- **Purpose:** Bit reinterpretation between unrelated types
- **Use cases:** Low-level system programming, hardware interfaces
- **Safety:** Very dangerous, avoid when possible

### 5. std::bit_cast (C++20)
- **Purpose:** Safe bit-level copying between types of same size
- **Use cases:** Type punning, serialization, bit manipulation
- **Safety:** Safe when types have same size and alignment

## Implementation Details

This sample demonstrates:

1. **static_cast Examples:** Numeric conversions, inheritance casting, void* restoration
2. **dynamic_cast Examples:** Polymorphic type checking with pointers and references
3. **const_cast Examples:** Adding/removing const (with warnings)
4. **reinterpret_cast Examples:** Bit manipulation (with strong warnings)
5. **std::bit_cast Examples:** Safe bit operations in C++20
6. **Best Practices:** Modern C++ casting guidelines

## When to Use Each Cast

### ✅ **Use static_cast when:**
- Converting between numeric types
- Navigating inheritance hierarchies (up/down casting when you know the type)
- Converting void* back to original pointer type
- Converting between related pointer types

### ✅ **Use dynamic_cast when:**
- You need runtime type checking for polymorphic objects
- Implementing type-safe downcasting
- Working with abstract base classes

### ⚠️ **Use const_cast when:**
- Calling legacy APIs that don't respect const-correctness
- You absolutely know the data isn't modified (rare)

### ❌ **Use reinterpret_cast when:**
- Working with hardware/OS interfaces
- Implementing low-level data structures
- You really know what you're doing (avoid if possible)

### ✅ **Use std::bit_cast when:**
- You need to reinterpret bits safely
- Types have the same size and alignment
- C++20 is available

## Performance Characteristics

| Cast Type | Performance | Safety | Use Case |
|-----------|-------------|--------|----------|
| static_cast | Fast | High | Most conversions |
| dynamic_cast | Slow | High | Polymorphic types |
| const_cast | Fast | Low | Legacy APIs |
| reinterpret_cast | Fast | Very Low | System programming |
| std::bit_cast | Fast | High | Bit manipulation |

**Performance Tips:**
- static_cast has zero runtime cost
- dynamic_cast requires RTTI and virtual functions
- Avoid casts in performance-critical code when possible
- Use smart pointers to avoid manual casting

## Common Patterns and Best Practices

### 1. Smart Pointer Casting
```cpp
// Instead of raw pointer casting
std::shared_ptr<Derived> derived = std::make_shared<Derived>();
std::shared_ptr<Base> base = derived;  // Automatic upcast

// For downcasting
std::shared_ptr<Base> base = /* ... */;
std::shared_ptr<Derived> derived = std::dynamic_pointer_cast<Derived>(base);
if (derived) {
    // Safe downcast succeeded
}
```

### 2. Safe Downcasting Pattern
```cpp
void processAnimal(Animal* animal) {
    if (auto dog = dynamic_cast<Dog*>(animal)) {
        dog->fetch();
    } else if (auto cat = dynamic_cast<Cat*>(animal)) {
        cat->scratch();
    } else {
        animal->speak();  // Fallback
    }
}
```

### 3. Numeric Conversion with Range Checking
```cpp
template <typename To, typename From>
std::optional<To> safe_cast(From value) {
    if (value >= std::numeric_limits<To>::min() &&
        value <= std::numeric_limits<To>::max()) {
        return static_cast<To>(value);
    }
    return std::nullopt;
}
```

### 4. Bit Manipulation with std::bit_cast
```cpp
// Safe float to bits conversion
float decomposeFloat(float value) {
    uint32_t bits = std::bit_cast<uint32_t>(value);
    // Extract sign, exponent, mantissa...
    return value;
}
```

## Casting vs Other Type Conversion Methods

| Method | When to Use | Advantages | Disadvantages |
|--------|-------------|------------|---------------|
| Cast Operators | Type conversion | Explicit, type-safe options | Can be misused |
| Constructor | Object creation | Clear intent | May create temporaries |
| User-defined conversion | Domain-specific | Semantic meaning | Hidden conversions |
| std::to_string, stoi, etc. | String ↔ numeric | Safe, throws on error | Only string/numeric |

## Testing Cast Operations

### 1. static_cast Testing
```cpp
TEST(CastTest, StaticCastNumeric) {
    double d = 3.14;
    int i = static_cast<int>(d);
    EXPECT_EQ(i, 3);
}
```

### 2. dynamic_cast Testing
```cpp
TEST(CastTest, DynamicCastSuccess) {
    Animal* animal = new Dog();
    Dog* dog = dynamic_cast<Dog*>(animal);
    ASSERT_NE(dog, nullptr);
    delete animal;
}

TEST(CastTest, DynamicCastFailure) {
    Animal* animal = new Cat();
    Dog* dog = dynamic_cast<Dog*>(animal);
    EXPECT_EQ(dog, nullptr);
    delete animal;
}
```

### 3. const_cast Testing (Limited)
```cpp
TEST(CastTest, ConstCast) {
    const int const_val = 42;
    int* ptr = const_cast<int*>(&const_val);
    // Don't modify! Just test the cast works
    EXPECT_EQ(*ptr, 42);
}
```

## Modern C++ Casting Guidelines

### 1. Prefer Smart Pointers
```cpp
// Good: No casting needed
std::unique_ptr<Dog> dog = std::make_unique<Dog>();
std::unique_ptr<Animal> animal = std::move(dog);
```

### 2. Use References for Polymorphism
```cpp
// Good: Reference casting
void processAnimal(Animal& animal) {
    try {
        Dog& dog = dynamic_cast<Dog&>(animal);
        dog.fetch();
    } catch (const std::bad_cast&) {
        // Handle failure
    }
}
```

### 3. Avoid C-Style Casts
```cpp
// Bad
int x = (int)3.14;

// Good
int x = static_cast<int>(3.14);
```

### 4. Document Cast Intent
```cpp
// Good: Document why the cast is safe
Base* base = getBasePointer();
// Safe: we know this Base* is actually a Derived* from factory function
Derived* derived = static_cast<Derived*>(base);
```

## Common Casting Mistakes to Avoid

### 1. Using Wrong Cast Type
```cpp
// Wrong: static_cast for polymorphic types
Animal* animal = new Dog();
Dog* dog = static_cast<Dog*>(animal);  // May be unsafe!

// Right: dynamic_cast for polymorphic types
Dog* dog = dynamic_cast<Dog*>(animal);  // Safe
```

### 2. Modifying Const Data
```cpp
// Wrong: Modifying truly const data
const int value = 42;
int* ptr = const_cast<int*>(&value);
*ptr = 100;  // UNDEFINED BEHAVIOR!

// Right: Only use const_cast for non-const data
int mutable_value = 42;
const int* const_ptr = &mutable_value;
int* mutable_ptr = const_cast<int*>(const_ptr);  // OK
```

### 3. Unsafe reinterpret_cast
```cpp
// Wrong: Unchecked reinterpretation
int i = 42;
double* d = reinterpret_cast<double*>(&i);  // Dangerous!

// Better: Use std::bit_cast if types are same size
auto bits = std::bit_cast<std::array<std::byte, sizeof(int)>>(i);
```

Choosing the right cast operator is crucial for writing safe, maintainable C++ code. Each cast has specific use cases and safety guarantees - use them appropriately!