# Move Semantics

## Overview

Move semantics is a C++11 feature that enables efficient transfer of resources from one object to another without expensive copying. This sample demonstrates the principles, best practices, and common patterns of move semantics in modern C++.

**The Problem:** Copying large objects (containers, strings, custom resources) is expensive and unnecessary when the source object won't be used again.

**The Solution:** Move semantics allow transferring ownership of resources, leaving the source object in a valid but unspecified state.

## How Move Semantics Works

1. **Rvalue References:** `T&&` denotes rvalue references that can bind to temporary objects
2. **Move Constructor:** `T(T&& other) noexcept` transfers resources from `other`
3. **Move Assignment:** `T& operator=(T&& other) noexcept` transfers resources
4. **std::move():** Converts lvalues to rvalues for moving
5. **Rule of Five:** Classes managing resources should implement all five special member functions

```cpp
class Resource {
    std::string data_;
public:
    // Move constructor
    Resource(Resource&& other) noexcept : data_(std::move(other.data_)) {}

    // Move assignment
    Resource& operator=(Resource&& other) noexcept {
        data_ = std::move(other.data_);
        return *this;
    }
};
```

## Sample Output

```
Running Move Semantics Sample...

=== Move Constructors ===

--- Creating original resource ---
Resource 'Original' constructed. Total instances: 1

--- Copy construction (expensive) ---
Resource 'Original_copy' copy-constructed from 'Original'. Total instances: 2

--- Move construction (efficient) ---
Resource 'Original' move-constructed. Total instances: 3

Original after move: ''
Copy: 'Original_copy'
Moved: 'Original'

=== Move Assignment ===

--- Creating resources ---
Resource 'Target' constructed. Total instances: 4
Resource 'Source' constructed. Total instances: 5

--- Copy assignment (expensive) ---
Resource 'CopyTarget' constructed. Total instances: 6
Resource 'Source_assigned' copy-assigned

--- Move assignment (efficient) ---
Resource 'Source' move-assigned

Source after move: ''
Target after move: 'Source'

=== Rule of Five ===

--- Resource class implements all five special member functions ---
1. Destructor
2. Copy constructor
3. Copy assignment operator
4. Move constructor
5. Move assignment operator

--- Demonstrating Rule of Five ---
Resource 'RuleOfFive_1' constructed. Total instances: 7
Resource 'RuleOfFive_1_copy' copy-constructed from 'RuleOfFive_1'. Total instances: 8
Resource 'RuleOfFive_1' move-constructed. Total instances: 9
Resource 'RuleOfFive_1' move-assigned
Resource 'RuleOfFive_1' destroyed. Total instances: 8
Resource 'RuleOfFive_1' destroyed. Total instances: 7
Resource 'RuleOfFive_1' destroyed. Total instances: 6

=== std::move Usage ===

--- When to use std::move ---
1. Moving from lvalues that won't be used again
2. Implementing move constructors/assignment
3. Passing ownership to containers or functions

--- Example: Moving into container ---
Resource 'TempResource' constructed. Total instances: 7
Resource 'TempResource' move-constructed. Total instances: 8
Temp after move: ''

--- Example: Function returning by value ---
Inside function, about to return
Resource 'LocalResource' constructed. Total instances: 9
Returned resource: 'LocalResource'

=== Move Semantics with Containers ===

--- Creating container with resources ---
Resource 'Res1' constructed. Total instances: 10
Resource 'Res2' constructed. Total instances: 11
Resource 'Res3' constructed. Total instances: 12

--- Moving entire container ---
Container1 size after move: 0
Container2 size after move: 3

--- Moving individual elements ---
Resource 'Individual' constructed. Total instances: 13
Resource 'Individual' move-constructed. Total instances: 14
Individual after move: ''

=== Perfect Forwarding ===

--- Perfect forwarding preserves value category ---
Passing lvalue: Argument type: lvalue reference
Resource 'Lvalue' constructed. Total instances: 15
Passing rvalue: Argument type: rvalue reference
Resource 'Rvalue' constructed. Total instances: 16
Passing moved lvalue: Argument type: rvalue reference
Resource 'Lvalue' move-constructed. Total instances: 17

=== RVO vs Move Semantics ===

--- Return Value Optimization (RVO) ---
Compiler can elide copies even without move semantics

--- When RVO doesn't apply ---
Calling function that benefits from RVO:
Inside function, about to return
Resource 'RVO_Resource' constructed. Total instances: 18
Returned resource: 'RVO_Resource'

Calling function where RVO doesn't apply:
Resource 'Res1' constructed. Total instances: 19
Resource 'Res2' constructed. Total instances: 20
Resource 'Res1' move-constructed. Total instances: 20
Resource 'Res1' destroyed. Total instances: 19
Resource 'Res2' destroyed. Total instances: 18

=== Move Semantics Best Practices ===

--- 1. Move-only types ---
MoveOnlyResource constructed
MoveOnlyResource move-constructed
Moved value: 42
MoveOnlyResource destroyed
MoveOnlyResource destroyed

--- 2. Don't move from const objects ---

--- 3. Be careful with self-assignment in move assignment ---

--- 4. Use noexcept for move operations ---
Move operations should be noexcept to enable optimizations

--- 5. Don't assume moved-from objects are empty ---
Moved-from objects should be in valid but unspecified state

=== Move Semantics Summary ===
Move semantics enable efficient resource transfer in C++11+
- Use std::move() to convert lvalues to rvalues
- Implement Rule of Five for classes managing resources
- Move operations should be noexcept when possible
- RVO can be better than move in some cases
- Perfect forwarding preserves value categories
- Move semantics work seamlessly with containers

Move semantics demonstration completed!
```

## Key Components

### 1. Move Constructors
- **Syntax:** `T(T&& other) noexcept`
- **Purpose:** Transfer resources from rvalues
- **Implementation:** Use `std::move()` on member variables
- **Noexcept:** Should be noexcept for optimization

### 2. Move Assignment Operators
- **Syntax:** `T& operator=(T&& other) noexcept`
- **Purpose:** Transfer resources from rvalues
- **Self-assignment:** Check for self-assignment
- **Cleanup:** Handle existing resources properly

### 3. Rule of Five
Modern C++ classes managing resources should implement:
- **Destructor:** Clean up resources
- **Copy Constructor:** Deep copy resources
- **Copy Assignment:** Deep copy with proper cleanup
- **Move Constructor:** Transfer resource ownership
- **Move Assignment:** Transfer resource ownership

### 4. std::move()
- **Purpose:** Convert lvalues to rvalues
- **Usage:** `std::move(object)` enables moving
- **Safety:** Only use when object won't be used again
- **Performance:** Enables move operations

## Implementation Details

This sample demonstrates:

1. **Move Constructors:** Efficient construction from rvalues
2. **Move Assignment:** Efficient assignment from rvalues
3. **Rule of Five:** Complete special member function implementation
4. **std::move Usage:** When and how to use std::move
5. **Container Operations:** Moving with standard containers
6. **Perfect Forwarding:** Preserving value categories
7. **RVO vs Move:** When compiler optimizations apply
8. **Best Practices:** Common patterns and pitfalls

## When to Use Move Semantics

### ✅ **Use move semantics when:**
- **Large objects:** Strings, vectors, custom resource managers
- **Temporary objects:** Function returns, temporary expressions
- **Reassignment:** When replacing object contents
- **Container insertions:** Moving elements into containers
- **Resource transfer:** Transferring ownership of resources

### ❌ **Don't use move semantics when:**
- **Small objects:** Trivial types where copy is cheaper
- **Const objects:** Can't move to/from const objects
- **Shared ownership:** Use shared_ptr for shared resources
- **Polymorphic objects:** Virtual functions complicate moving

## Performance Characteristics

| Operation | Cost | Use Case |
|-----------|------|----------|
| Copy Construction | High (O(n)) | Need independent copies |
| Move Construction | Low (O(1)) | Transfer ownership |
| Copy Assignment | High (O(n)) | Replace with independent copy |
| Move Assignment | Low (O(1)) | Replace with ownership transfer |

**Performance Tips:**
- Move operations should be `noexcept`
- Prefer move over copy when possible
- RVO can eliminate even move operations
- Profile to identify bottlenecks

## Common Patterns and Best Practices

### 1. Resource Management Class
```cpp
class ResourceManager {
    std::unique_ptr<Resource> resource_;
public:
    ResourceManager(ResourceManager&& other) noexcept
        : resource_(std::move(other.resource_)) {}

    ResourceManager& operator=(ResourceManager&& other) noexcept {
        resource_ = std::move(other.resource_);
        return *this;
    }
};
```

### 2. Move-Only Types
```cpp
class MoveOnly {
    std::unique_ptr<int> data_;
public:
    MoveOnly() = default;
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
};
```

### 3. Container with Move Semantics
```cpp
std::vector<MyClass> vec;
MyClass obj;
// Efficient: move into container
vec.push_back(std::move(obj));
// obj is now in moved-from state
```

### 4. Function Parameters
```cpp
void process_data(std::vector<int> data) {
    // data is moved into function
    // process...
}

std::vector<int> create_data();
// Efficient: move temporary into function
process_data(create_data());
```

## Perfect Forwarding

Perfect forwarding preserves the value category of arguments:

```cpp
template <typename T>
void wrapper(T&& arg) {
    // arg is lvalue reference if T is lvalue
    // arg is rvalue reference if T is rvalue
    target_function(std::forward<T>(arg));
}
```

## RVO (Return Value Optimization)

The compiler can eliminate copies/moves in return statements:

```cpp
MyClass create_object() {
    MyClass local;
    // ... initialize ...
    return local;  // No copy/move with RVO
}
```

## Move Semantics and Exceptions

- **Move constructors should be `noexcept`**
- **Strong exception guarantee:** Operations should succeed or fail completely
- **Container operations:** Use move when exception safety allows

## Testing Move Semantics

### 1. Verify Move Operations
```cpp
TEST(MyClass, MoveConstructor) {
    MyClass original = create_object();
    MyClass moved = std::move(original);

    // Verify moved-from state
    EXPECT_TRUE(original.empty());
    // Verify moved-to state
    EXPECT_FALSE(moved.empty());
}
```

### 2. Performance Testing
```cpp
TEST(MyClass, MoveVsCopyPerformance) {
    MyClass large_object = create_large_object();

    auto copy_start = std::chrono::high_resolution_clock::now();
    MyClass copy = large_object;
    auto copy_end = std::chrono::high_resolution_clock::now();

    auto move_start = std::chrono::high_resolution_clock::now();
    MyClass moved = std::move(large_object);
    auto move_end = std::chrono::high_resolution_clock::now();

    EXPECT_LT(move_time, copy_time);
}
```

### 3. Exception Safety
```cpp
TEST(MyClass, MoveNoexcept) {
    EXPECT_TRUE(noexcept(MyClass(std::declval<MyClass&&>())));
    EXPECT_TRUE(noexcept(std::declval<MyClass&>() = std::declval<MyClass&&>()));
}
```

## Advanced Usage Patterns

### 1. Move-Aware Algorithms
```cpp
template <typename Container>
void move_elements(Container& dest, Container& src) {
    dest.reserve(dest.size() + src.size());
    for (auto& elem : src) {
        dest.push_back(std::move(elem));
    }
    src.clear();
}
```

### 2. Conditional Move
```cpp
MyClass select_best(MyClass a, MyClass b) {
    if (a.score() > b.score()) {
        return a;  // Move a
    } else {
        return b;  // Move b
    }
    // RVO might apply, otherwise move
}
```

### 3. Move Iterator
```cpp
std::vector<MyClass> vec1 = create_objects();
std::vector<MyClass> vec2(std::make_move_iterator(vec1.begin()),
                          std::make_move_iterator(vec1.end()));
```

Move semantics is fundamental to modern C++ performance. Understanding when and how to use move operations enables writing efficient code that minimizes unnecessary copying while maintaining correctness and safety.