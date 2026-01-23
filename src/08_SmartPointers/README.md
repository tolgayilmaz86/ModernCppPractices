# Smart Pointers

## Overview

Smart pointers are one of the most important features of Modern C++, providing automatic memory management and preventing common memory-related bugs like leaks, double deletions, and dangling pointers. This sample demonstrates the three main smart pointer types: `std::unique_ptr`, `std::shared_ptr`, and `std::weak_ptr`, along with best practices and common pitfalls.

**The Problem:** Manual memory management with `new` and `delete` is error-prone, leading to memory leaks, dangling pointers, and undefined behavior.

**The Solution: Smart Pointers** RAII-based automatic memory management that ensures resources are properly cleaned up when they go out of scope.

## How Smart Pointers Work

1. **RAII Principle:** Resource acquisition is initialization - smart pointers acquire resources in constructors and release them in destructors
2. **Ownership Semantics:** Different smart pointers provide different ownership models
3. **Automatic Cleanup:** No manual `delete` calls needed
4. **Exception Safety:** Resources are cleaned up even when exceptions occur

```cpp
// Old way - error prone
Resource* ptr = new Resource();
ptr->use();
// Forgot to delete? Memory leak!
// Exception thrown? Memory leak!

// New way - automatic and safe
auto ptr = std::make_unique<Resource>();
ptr->use();
// Automatically deleted when ptr goes out of scope
```

## Sample Output

```
Running Smart Pointers Sample...

=== std::unique_ptr - Exclusive Ownership ===
Creating unique_ptr...
Resource 'UniqueResource' created. Total instances: 1
Using resource 'UniqueResource'
Transferring ownership...
res1 is now null
Using resource 'UniqueResource'
Using custom deleter...
Resource 'CustomDelete' created. Total instances: 2
Using resource 'CustomDelete'
Verbose deleter: cleaning up 'CustomDelete'
Resource 'CustomDelete' destroyed. Total instances: 1
unique_ptr demonstration completed

=== std::shared_ptr - Shared Ownership ===
Creating shared_ptr...
Resource 'SharedResource' created. Total instances: 2
Use count: 1
After sharing - Use count: 2
Using resource 'SharedResource'
After another share - Use count: 3
Using resource 'SharedResource'
After res3 destroyed - Use count: 2
After res2 destroyed - Use count: 1
Using custom deleter with shared_ptr...
Resource 'SharedCustom' created. Total instances: 3
Using resource 'SharedCustom'
Custom deleter called for resource 'SharedCustom'
Resource 'SharedCustom' destroyed. Total instances: 2
shared_ptr demonstration completed

=== std::weak_ptr - Non-owning References ===
Shared use count: 1
After creating weak_ptr - Shared use count: 1
Weak pointer locked successfully
Using resource 'WeakDemo'
Resetting shared_ptr...
Weak pointer expired after shared_ptr reset
weak_ptr demonstration completed

=== Circular References Problem ===
Creating circular reference with shared_ptr...
Parent use count: 1
Child use count: 1
Objects should be destroyed now, but circular reference prevents it!
Total Resource instances: 2

Solution: Using weak_ptr to break cycles...
WeakParent use count: 1
WeakChild use count: 1
WeakParent destroyed
WeakChild destroyed
Objects properly destroyed with weak_ptr solution

=== Array Support ===
unique_ptr for arrays...
Resource 'Array1' created. Total instances: 3
Resource 'Array2' created. Total instances: 4
Resource 'Array3' created. Total instances: 5
Using resource 'Array1'
Using resource 'Array2'
Using resource 'Array3'
Resource 'Array3' destroyed. Total instances: 4
Resource 'Array2' destroyed. Total instances: 3
Resource 'Array1' destroyed. Total instances: 2

shared_ptr for arrays...
Resource 'SharedArray1' created. Total instances: 3
Resource 'SharedArray2' created. Total instances: 4
Using resource 'SharedArray1'
Using resource 'SharedArray2'
Resource 'SharedArray2' destroyed. Total instances: 3
Resource 'SharedArray1' destroyed. Total instances: 2
Array demonstration completed

=== Best Practices and Common Pitfalls ===
Using resource 'GoodUnique'
Using resource 'GoodShared'
Raw pointer considerations:
Using resource 'GoodUnique'
File 'example.txt' opened
Writing to file 'example.txt': Hello, smart pointers!
File 'example.txt' closed
Best practices demonstration completed

=== Performance Considerations ===
Memory overhead:
- unique_ptr: Minimal (usually same as raw pointer)
- shared_ptr: Reference counter (typically 16-32 bytes)
- weak_ptr: Additional weak count in control block

Performance characteristics:
- make_shared: Allocates object and control block together
- shared_ptr from unique_ptr: Creates new control block
- weak_ptr.lock(): Atomic operations for thread safety

make_shared vs new + shared_ptr:
Resource 'Efficient' created. Total instances: 5
Resource 'Efficient' destroyed. Total instances: 4
Resource 'Inefficient' created. Total instances: 5
Resource 'Inefficient' destroyed. Total instances: 4
Performance demonstration completed

=== Smart Pointers Summary ===
std::unique_ptr:
- Exclusive ownership
- Zero overhead
- Cannot be copied
- Use for sole ownership

std::shared_ptr:
- Shared ownership
- Reference counting
- Thread-safe reference counting
- Use when multiple owners needed

std::weak_ptr:
- Non-owning reference
- Breaks circular references
- Must be locked to access
- Use for observation without ownership

When to use raw pointers:
- Non-owning observations
- Performance-critical code
- Interfacing with C APIs
- Inside smart pointer implementations

Smart pointers best practices:
- Prefer make_unique and make_shared
- Use unique_ptr by default
- Avoid circular references
- Be careful with raw pointers from get()
- Use custom deleters when necessary

Smart pointers demonstration completed!
```

## Key Components

### 1. std::unique_ptr
- **Exclusive ownership** - only one pointer can own the resource
- **Zero overhead** - same performance as raw pointers
- **Move-only** - cannot be copied, only moved
- **Custom deleters** supported for special cleanup needs
- **Array support** with `[]` operator

### 2. std::shared_ptr
- **Shared ownership** - multiple pointers can share ownership
- **Reference counting** - automatically deletes when last owner is destroyed
- **Thread-safe** reference counting (but not the pointed-to object)
- **Control block overhead** for reference counting
- **make_shared** optimization for better memory layout

### 3. std::weak_ptr
- **Non-owning reference** to shared objects
- **Breaks circular references** between shared_ptr objects
- **Must be locked** to access the pointed-to object
- **Thread-safe** operations
- **Observer pattern** implementation helper

## Implementation Details

This sample demonstrates:

1. **Basic Usage**: Creating and using each smart pointer type
2. **Ownership Transfer**: Moving unique_ptr, sharing with shared_ptr
3. **Custom Deleters**: Function pointers and functors for cleanup
4. **Circular References**: The problem and weak_ptr solution
5. **Array Support**: Smart pointers for dynamic arrays
6. **Best Practices**: Common patterns and pitfalls to avoid
7. **Performance**: Memory overhead and optimization considerations

## When to Use Each Smart Pointer

### std::unique_ptr
✅ **Always prefer by default**
- Single ownership scenarios
- RAII resource management
- Factory functions returning ownership
- Private implementation details (PIMPL)
- STL containers and algorithms

### std::shared_ptr
✅ **When multiple ownership is needed**
- Observer pattern implementations
- Shared resources across threads
- Plugin architectures
- Cache implementations
- Graph data structures with cycles (with weak_ptr)

### std::weak_ptr
✅ **To break ownership cycles**
- Observer pattern (non-owning observation)
- Cache invalidation
- Breaking shared_ptr cycles
- Optional references to shared resources

### Raw Pointers
✅ **Only when necessary**
- Non-owning observations
- Performance-critical inner loops
- C API interoperability
- Inside smart pointer implementations

## Performance Characteristics

| Smart Pointer | Memory Overhead | Performance | Thread Safety |
|---------------|-----------------|-------------|---------------|
| unique_ptr | None | Same as raw | N/A |
| shared_ptr | 16-32 bytes | Reference counting | Reference count |
| weak_ptr | +8 bytes | Lock operation | Operations |

**Key Performance Tips:**
- Use `make_unique` and `make_shared` for exception safety
- `make_shared` allocates object and control block together
- Avoid creating `shared_ptr` from `unique_ptr` (creates new control block)
- `weak_ptr.lock()` involves atomic operations
- Prefer `unique_ptr` when possible for zero overhead

## Common Pitfalls and Solutions

### 1. Circular References
```cpp
// Problem
struct A { std::shared_ptr<B> b; };
struct B { std::shared_ptr<A> a; };
// A and B keep each other alive

// Solution
struct B { std::weak_ptr<A> a; };  // Break the cycle
```

### 2. Raw Pointers from get()
```cpp
auto ptr = std::make_unique<int>(42);
int* raw = ptr.get();
// Don't delete raw! Don't store it long-term!
```

### 3. Exception Safety
```cpp
// Bad - exception between new and shared_ptr construction
func(std::shared_ptr<T>(new T), other_args);  // Leak if other_args throws

// Good - make_shared is exception safe
func(std::make_shared<T>(), other_args);
```

### 4. Arrays
```cpp
// For single objects
auto ptr = std::make_unique<T>();

// For arrays
auto arr = std::unique_ptr<T[]>(new T[size]);
```

## Advanced Usage Patterns

### 1. Custom Deleters
```cpp
// Function deleter
auto file = std::unique_ptr<FILE, decltype(&fclose)>(fopen("file.txt", "r"), fclose);

// Functor deleter
struct ResourceDeleter {
    void operator()(Resource* r) { /* custom cleanup */ }
};
std::unique_ptr<Resource, ResourceDeleter> ptr(new Resource, ResourceDeleter{});
```

### 2. PIMPL Idiom
```cpp
class Widget {
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
public:
    Widget();
    ~Widget();
    // No need for copy/move declarations - unique_ptr handles it
};
```

### 3. Factory Functions
```cpp
std::unique_ptr<Shape> create_circle(double radius) {
    return std::make_unique<Circle>(radius);
}

std::shared_ptr<Texture> load_texture(const std::string& path) {
    return std::make_shared<Texture>(path);
}
```

## Best Practices

1. **Prefer `std::unique_ptr`** by default for single ownership
2. **Use `std::make_unique` and `std::make_shared`** for exception safety
3. **Avoid raw `new` and `delete`** in application code
4. **Use `std::weak_ptr`** to break circular references
5. **Be careful with `.get()`** - treat as temporary observation only
6. **Consider ownership semantics** when choosing pointer types
7. **Use custom deleters** when resources need special cleanup
8. **Avoid `shared_ptr`** for performance-critical code when possible

## Modern C++ Integration

Smart pointers integrate seamlessly with other Modern C++ features:

- **Move semantics** for efficient ownership transfer
- **RAII** for automatic resource management
- **Lambda expressions** for custom deleters
- **Template metaprogramming** for generic resource management
- **STL containers** that work with movable types
- **Exception safety guarantees** throughout the standard library

Smart pointers are fundamental to writing safe, modern C++ code. They eliminate entire classes of memory management bugs while maintaining high performance and providing clear ownership semantics.