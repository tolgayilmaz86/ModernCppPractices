# Deep Copy vs Shallow Copy

## Overview

### The Problem: What Happens When You Copy?

When you copy an object, what should happen to the pointers inside it?

**Shallow Copy (Default Behavior)**
Just copies the pointer value - now both objects point to the SAME data. This leads to:
- Double-free bugs (both try to delete the same memory)
- Unexpected changes (modifying one affects the other)

```cpp
class ShallowBuffer {
    int* data;
public:
    ShallowBuffer() : data(new int[100]) {}
    ~ShallowBuffer() { delete[] data; }
    // Default copy constructor does: newObj.data = oldObj.data
    // Now BOTH objects point to the SAME array!
};

ShallowBuffer a;
ShallowBuffer b = a;  // Shallow copy: b.data points to same memory as a.data
// When b is destroyed: deletes data
// When a is destroyed: tries to delete SAME data again = CRASH!
```

**Deep Copy (Safe Behavior)**
Creates a completely independent copy - each object owns its own data.

```cpp
class DeepBuffer {
    size_t size;
    std::unique_ptr<int[]> data;
    
public:
    DeepBuffer(size_t s = 100) : size(s), data(std::make_unique<int[]>(s)) {}
    
    // Deep copy: allocate NEW memory and copy contents
    DeepBuffer(const DeepBuffer& other) 
        : size(other.size)
        , data(std::make_unique<int[]>(other.size)) 
    {
        std::copy(other.data.get(), other.data.get() + size, data.get());
    }
    
    // Copy assignment with copy-and-swap idiom (exception safe!)
    DeepBuffer& operator=(const DeepBuffer& other) {
        if (this != &other) {
            DeepBuffer temp(other);  // Make a copy first
            std::swap(size, temp.size);
            data.swap(temp.data);    // Swap with temp (temp cleans up old data)
        }
        return *this;
    }
    
    // Move operations: transfer ownership (fast, no copying)
    DeepBuffer(DeepBuffer&&) noexcept = default;
    DeepBuffer& operator=(DeepBuffer&&) noexcept = default;
};
```