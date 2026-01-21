# Rule of Five

## Overview

The Rule of Five (also known as Rule of Three/Five) states that if a class defines any of the following special member functions, it should probably define all five:

1. **Copy Constructor**
2. **Copy Assignment Operator**
3. **Move Constructor**
4. **Move Assignment Operator**
5. **Destructor**

This rule exists because classes that manage resources (memory, files, sockets, etc.) typically need custom implementations for all these operations to handle resource ownership correctly.

**The Problem:** When a class manages resources but doesn't implement all special member functions, it leads to bugs like:
- Double deletion of resources
- Resource leaks
- Dangling pointers
- Undefined behavior

**The Solution: Rule of Five** Implement all five special member functions when a class manages resources.

## Special Member Functions

### 1. Copy Constructor
```cpp
MyClass(const MyClass& other) {
    // Deep copy resources from other
}
```

### 2. Copy Assignment Operator
```cpp
MyClass& operator=(const MyClass& other) {
    if (this != &other) {
        // Clean up existing resources
        // Deep copy resources from other
    }
    return *this;
}
```

### 3. Move Constructor
```cpp
MyClass(MyClass&& other) noexcept {
    // Transfer ownership of resources from other
    // Leave other in valid but unspecified state
}
```

### 4. Move Assignment Operator
```cpp
MyClass& operator=(MyClass&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        // Transfer ownership from other
    }
    return *this;
}
```

### 5. Destructor
```cpp
~MyClass() {
    // Clean up resources
}
```

## When to Follow the Rule

✅ **Must follow when:**
- Class manages heap memory (`new`/`delete`)
- Class owns file handles, sockets, or other system resources
- Class contains `std::unique_ptr` members
- Class has members that follow Rule of Five

❌ **Can use defaults when:**
- Class only contains Plain Old Data (POD)
- All members have proper copy/move semantics
- No resource management needed

## Sample Output

```
Running Rule of Five Sample...

=== HeapResource (Proper Rule of Five) ===
HeapResource: Allocated 10 bytes
heap1[0] = A, heap1[1] = B
HeapResource: Deep copied 10 bytes
heap2[0] = A, heap2[1] = B
HeapResource: Allocated 5 bytes
HeapResource: Deep copy assigned 10 bytes
heap3[0] = A, heap3[1] = B
HeapResource: Moved 10 bytes
HeapResource: Allocated 3 bytes
HeapResource: Move assigned 10 bytes
HeapResource: Deallocated 10 bytes
HeapResource: Deallocated 10 bytes
HeapResource: Deallocated 10 bytes
HeapResource: Deallocated 10 bytes
HeapResource: Deallocated 10 bytes

=== FileResource (Proper Rule of Five) ===
FileResource: Opened file 'test1.txt'
FileResource: Copy created file 'test1.txt_copy'
FileResource: Opened file 'test3.txt'
FileResource: Copy assigned file 'test1.txt_assigned'
FileResource: Moved file 'test1.txt_copy'
FileResource: Opened file 'test5.txt'
FileResource: Move assigned file 'test1.txt_assigned'
FileResource: Closed file 'test1.txt'
FileResource: Closed file 'test1.txt_copy'
FileResource: Closed file 'test3.txt'
FileResource: Closed file 'test5.txt'
FileResource: Closed file 'test1.txt_assigned'

=== BrokenResource (Violates Rule of Five) ===
This will demonstrate undefined behavior!
BrokenResource: Created with value 42
broken1 value: 42
Avoiding the copy to prevent crash...
BrokenResource: Destroyed

=== ProperResource (Follows Rule of Five) ===
ProperResource: Created with value 100
proper1 value: 100
ProperResource: Deep copied with value 100
proper2 value: 100
ProperResource: Created with value 50
ProperResource: Deep copy assigned with value 100
proper3 value: 100
ProperResource: Moved
ProperResource: Created with value 25
ProperResource: Move assigned
ProperResource: Destroyed
ProperResource: Destroyed
ProperResource: Destroyed
ProperResource: Destroyed
ProperResource: Destroyed

Rule of Five demonstration completed!
Key takeaways:
- If a class manages resources, implement all 5 special member functions
- Copy operations should perform deep copies
- Move operations should leave source in valid but unspecified state
- Violating the rule leads to resource leaks, double deletion, or crashes
```

## Implementation Details

This sample demonstrates:

1. **HeapResource**: Manages C-style heap memory with proper Rule of Five
2. **FileResource**: Manages file handles with proper Rule of Five
3. **BrokenResource**: Shows what happens when Rule of Five is violated
4. **ProperResource**: Correct implementation following Rule of Five

The sample shows both the correct implementations and the problems that occur when the rule is not followed, making it clear why this rule is essential for robust C++ code.</content>
<parameter name="filePath">d:\repos\ModernCppPractices\src\05_RuleOfFive\README.md