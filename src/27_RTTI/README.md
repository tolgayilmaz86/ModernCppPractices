# RTTI (Run-Time Type Information)

## Overview

RTTI (Run-Time Type Information) is a C++ feature that allows determining the type of an object at runtime. It provides two main mechanisms: `typeid` for obtaining type information and `dynamic_cast` for safe downcasting in polymorphic class hierarchies.

**The Problem:** Sometimes you need to know the actual derived type of an object when you only have a base class pointer or reference. Without RTTI, you would need to implement custom type identification mechanisms.

**The Solution:** C++ provides built-in RTTI through `typeid` and `dynamic_cast`, which work automatically with polymorphic types.

## How RTTI Works

1. **typeid operator:** Returns a `std::type_info` object containing type information
2. **dynamic_cast operator:** Performs runtime-checked downcasting
3. **std::type_info class:** Contains type name and comparison capabilities

```cpp
// Basic typeid usage
int x = 42;
std::cout << typeid(x).name() << "\n";  // Prints: int (or mangled name)

// Polymorphic typeid (runtime)
Shape* shape = new Circle(5.0);
std::cout << typeid(*shape).name() << "\n";  // Prints: Circle (or mangled)

// Safe downcasting with dynamic_cast
if (Circle* circle = dynamic_cast<Circle*>(shape)) {
    // Safe to use circle
}
```

## RTTI Across C++ Standards

### C++14: Basic RTTI

C++14 provides the foundational RTTI features:

```cpp
// typeid operator
const std::type_info& info = typeid(some_object);

// dynamic_cast for pointers (returns nullptr on failure)
Derived* d = dynamic_cast<Derived*>(base_ptr);

// dynamic_cast for references (throws std::bad_cast on failure)
Derived& d = dynamic_cast<Derived&>(base_ref);

// type_info operations
if (typeid(a) == typeid(b)) { /* same type */ }
std::cout << info.name();        // Implementation-defined name
std::cout << info.hash_code();   // Hash for unordered containers
```

### C++17: std::any and RTTI

C++17 introduced `std::any`, which internally uses RTTI for type-safe storage:

```cpp
#include <any>

std::any a = 42;
std::cout << a.type().name() << "\n";  // Uses RTTI

// Type-safe access
try {
    int value = std::any_cast<int>(a);  // OK
    double bad = std::any_cast<double>(a);  // Throws std::bad_any_cast
} catch (const std::bad_any_cast& e) {
    std::cout << "Type mismatch: " << e.what() << "\n";
}

// Pointer version (returns nullptr on failure)
if (int* ptr = std::any_cast<int>(&a)) {
    std::cout << "Value: " << *ptr << "\n";
}
```

**Note:** `std::any` requires RTTI to be enabled. Disabling RTTI (`-fno-rtti`) will make `std::any` unavailable.

### C++20: Concepts as RTTI Alternative

C++20 introduced concepts, which provide compile-time type checking as an alternative to runtime RTTI:

```cpp
#include <concepts>

// Instead of runtime type checking with RTTI
template<typename T>
void old_process(T& obj) {
    if (typeid(obj) == typeid(Derived)) {
        // Process as Derived
    }
}

// Use concepts for compile-time checking
template<typename T>
requires std::derived_from<T, Base>
void new_process(T& obj) {
    // Guaranteed to be derived from Base at compile time
    obj.base_function();
}

// Or with abbreviated syntax
void process(std::derived_from<Base> auto& obj) {
    obj.base_function();
}
```

C++20 also added `std::type_identity` for controlling template argument deduction:

```cpp
#include <type_traits>

// Prevents template argument deduction
template<typename T>
void foo(std::type_identity_t<T> arg);

// Usage: Must explicitly specify T
foo<int>(42);  // OK
foo(42);       // Error: cannot deduce T
```

## Sample Output

```
Running RTTI (Run-Time Type Information) Sample...

=== typeid Operator ===
typeid(int).name(): int
typeid(double).name(): double
typeid(const char*).name(): PKc

typeid(i).name(): i
typeid(d).name(): d
typeid(str).name(): PKc

Polymorphic typeid:
Static type (Shape*): P7Shape
Dynamic type (*shape): 6Circle

Comparing types:
shapeRef1 is Circle: Yes
shapeRef2 is Circle: No
shapeRef1 == shapeRef2: No

=== dynamic_cast Operator ===
Processing Circle:
  Area: 12.5664
  Successfully cast to Circle!
  Radius: 2

Processing Square:
  Area: 9
  Successfully cast to Square!
  Side: 3

Processing Triangle:
  Area: 10
  Successfully cast to Triangle!
  Sides: 3

Processing Circle:
  Area: 7.06858
  Successfully cast to Circle!
  Radius: 1.5

--- dynamic_cast with references ---
Reference cast to Circle successful!
Radius: 2
Expected bad_cast for Square->Circle: std::bad_cast

=== std::type_info Details ===
int type name: i
Circle type name: 6Circle

hash_code() for int: 123456789
hash_code() for Circle: 987654321

type_info comparisons:
int == double: false
int == int: true
int != double: true

=== RTTI Limitations and Considerations ===
1. RTTI only works with polymorphic classes (at least one virtual function)
   Non-polymorphic types use static (compile-time) type info

2. RTTI can be disabled with compiler flags (-fno-rtti on GCC/Clang)
   This is common in embedded systems to reduce binary size

3. RTTI has runtime overhead:
   - Type information stored in vtable
   - String comparisons for type names
   - dynamic_cast traverses inheritance hierarchy

4. type_info::name() is implementation-defined:
   - May be mangled (GCC/Clang) or readable (MSVC)
   - Not portable across compilers

=== C++17: std::any and RTTI ===
std::any stores any CopyConstructible type:
a1 type: i
a2 type: d
a3 type: NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
a4 type: 6Circle

std::any_cast with type checking:
a1 as int: 42
Expected bad_any_cast (int != double): std::bad_any_cast
Pointer access successful: 42
Pointer access returned nullptr (correct)

=== C++20: RTTI-Related Features ===
C++20 std::type_identity:
  Useful for preventing template argument deduction
  Example: void foo(std::type_identity_t<T> arg)

C++20 Concepts as RTTI alternative:
  Concepts check types at compile-time instead of runtime
  This is more efficient and catches errors earlier

=== RTTI Best Practices ===
1. Prefer virtual functions over explicit type checking
   BAD: if (typeid(obj) == typeid(Derived)) { ... }
   GOOD: obj.virtualFunction();

2. Use dynamic_cast for safe downcasting when needed
   - Always check the result (nullptr for pointers)
   - Catch std::bad_cast for references

3. Consider alternatives to RTTI:
   - Virtual functions (polymorphism)
   - Visitor pattern
   - std::variant with std::visit (C++17)
   - Concepts (C++20) for compile-time checks

RTTI demonstration completed!
```

## Key Components

### 1. typeid Operator

- **Purpose:** Obtain type information at runtime
- **Returns:** `const std::type_info&` object
- **Behavior:**
  - For polymorphic types: Returns actual (dynamic) type
  - For non-polymorphic types: Returns static (declared) type

```cpp
// Polymorphic type - runtime check
Shape* shape = new Circle();
std::cout << typeid(*shape).name();  // "Circle"

// Non-polymorphic type - compile-time
int x = 42;
std::cout << typeid(x).name();  // "int"
```

### 2. dynamic_cast Operator

- **Purpose:** Safe downcasting in polymorphic hierarchies
- **Pointer form:** Returns `nullptr` on failure
- **Reference form:** Throws `std::bad_cast` on failure

```cpp
// Pointer form - check for nullptr
if (Circle* c = dynamic_cast<Circle*>(shape)) {
    c->circleSpecificMethod();
}

// Reference form - catch exception
try {
    Circle& c = dynamic_cast<Circle&>(shape_ref);
} catch (const std::bad_cast& e) {
    // Handle cast failure
}
```

### 3. std::type_info Class

- **name():** Implementation-defined type name
- **hash_code():** Hash value for use in containers (You need to define your own hash function for type_info if you want to use it in unordered containers)
- **before():** Implementation-defined ordering
- **operator==/!=:** Type comparison

```cpp
const std::type_info& info = typeid(MyClass);
std::cout << info.name() << "\n";
std::cout << info.hash_code() << "\n";
```

## RTTI vs Alternatives

| Feature | RTTI | Virtual Functions | std::variant | Concepts |
|---------|------|-------------------|--------------|----------|
| Type Safety | Runtime | Compile-time | Compile-time | Compile-time |
| Performance | Overhead | Minimal | Minimal | Zero |
| Flexibility | High | Medium | Medium | High |
| C++ Version | C++98+ | C++98+ | C++17+ | C++20+ |
| RTTI Required | Yes | No | No | No |

## When to Use RTTI

### Good Use Cases

1. **Debugging and Logging:** Type names for diagnostics
2. **Serialization Frameworks:** Type identification for serialization
3. **Plugin Systems:** Dynamic type loading and identification
4. **When Virtual Functions Aren't Sufficient:** Complex type-dependent logic

### Avoid RTTI When

1. **Simple Polymorphism Suffices:** Use virtual functions instead
2. **Performance is Critical:** RTTI has runtime overhead
3. **Embedded Systems:** RTTI increases binary size
4. **Cross-Boundary Type Checking:** RTTI may not work across shared libraries

## Best Practices

1. **Prefer Virtual Functions:** Use polymorphism instead of explicit type checking
2. **Check dynamic_cast Results:** Always handle nullptr or std::bad_cast
3. **Consider Alternatives:** std::variant, visitor pattern, or concepts
4. **Cache Type Info:** Avoid repeated typeid calls in hot paths
5. **Document RTTI Usage:** Make it clear why RTTI is necessary

## Compiler Support

- **Enable RTTI:** Default in most compilers
- **Disable RTTI:** `-fno-rtti` (GCC/Clang), `/GR-` (MSVC)
- **Check RTTI:** `__cpp_rtti` macro (C++23)

```cpp
#if __cpp_rtti
    std::cout << "RTTI is enabled\n";
#else
    std::cout << "RTTI is disabled\n";
#endif
```

## Related Topics

- **[Casting Types](../14_CastingTypes/README.md):** All C++ cast operators
- **[Type Erasure](../06_TypeErasure/README.md):** Hiding type information
- **[Concepts](../16_Concepts/README.md):** Compile-time type constraints
- **[Variant Visitor](../07_VariantVisitor/README.md):** Type-safe alternatives to RTTI
