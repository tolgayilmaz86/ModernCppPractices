# Pimpl (Pointer to Implementation)

## Overview

Pimpl, short for "Pointer to Implementation", is a C++ idiom that hides implementation details of a class from its public interface. The class contains only a pointer to a separate implementation class that contains all the actual data members and method implementations.

**The Problem:** When a class's implementation changes, all code that includes its header must be recompiled. This creates tight coupling between implementation and interface. Also sometimes if you only ship header files for your library/APIs/SDKs, you may want to hide implementation details for security or intellectual property reasons.

**The Solution: Pimpl** Separate the interface from the implementation using a pointer. Changes to implementation don't require recompiling client code.

## Key Benefits

- **Reduced Compilation Dependencies**: Clients only depend on the public interface
- **Binary Compatibility**: Implementation changes don't break existing binaries
- **Information Hiding**: Implementation details are completely hidden
- **Faster Compilation**: Smaller header files, less transitive includes
- **Implementation Flexibility**: Can change implementation without affecting interface

## Basic Structure

```cpp
// widget.hpp - Public interface
class Widget {
private:
    class SDKWidget;  // Forward declaration
    std::unique_ptr<SDKWidget> _sdkWidget;
public:
    Widget();
    ~Widget();
    // ... public methods
};

// widget.cpp - Implementation
class Widget::SDKWidget {
    // All data members and private methods
};

Widget::Widget() : _sdkWidget(std::make_unique<SDKWidget>()) {}
Widget::~Widget() = default;
// ... method implementations
```

## Important Considerations

### Copy Semantics
**Must implement copy constructor and assignment operator to ensure deep copying:**

```cpp
Widget::Widget(const Widget& other)
    : _sdkWidget(std::make_unique<SDKWidget>(*other._sdkWidget)) {}

Widget& Widget::operator=(const Widget& other) {
    _sdkWidget = std::make_unique<SDKWidget>(*other._sdkWidget);
    return *this;
}
```

### Move Semantics
Can be defaulted in and from C++11+:

```cpp
Widget::Widget(Widget&& other) noexcept = default;
Widget& Widget::operator=(Widget&& other) noexcept = default;
```

### Exception Safety
The Rule of Zero/Three/Five applies. With `std::unique_ptr`, destructor is automatically noexcept.

## When to Use Pimpl

✅ **Good for:**
- Library interfaces (ABI stability crucial)
- Classes with heavy implementation dependencies
- Classes likely to change implementation frequently
- Reducing build times in large projects

❌ **Not good for:**
- Simple classes with few dependencies
- Performance-critical code (slight indirection overhead)
- Classes where copying is very expensive
- Template classes (implementation must be visible)

## Sample Output

```
Running Pimpl Sample...

=== Basic Pimpl Usage ===
Created widget: MyWidget with value 5
Added data to widget:
Data: [10, 20, 30]
Data processed (multiplied by 5)
Data: [50, 100, 150]

=== Copy Semantics ===
Copied widget: MyWidget with value 5
Data: [50, 100, 150]
Modified copy: CopiedWidget with value 10
Original: MyWidget with value 5

=== Move Semantics ===
Moved widget: CopiedWidget with value 10
Data: [50, 100, 150]
Source after move:  with value 0

=== Public Interface Example ===
Data: Hello Pimpl!
Counter: 2
Original - Data: Hello Pimpl!, Counter: 2
Copy - Data: Copied!, Counter: 3

Pimpl demonstration completed!
Benefits of Pimpl:
- Implementation details hidden from public interface
- Reduced compilation dependencies
- Binary compatibility (ABI stability)
- Implementation can change without affecting clients
```

## Implementation Details

This sample demonstrates:

1. **Basic Pimpl Structure**: Widget class with separate implementation
2. **Copy Semantics**: Deep copying of implementation
3. **Move Semantics**: Efficient move operations
4. **Public Interface**: Hiding complex implementation behind simple interface
5. **Exception Safety**: Proper resource management

The Pimpl pattern is essential for creating maintainable C++ libraries and reducing compilation times in large codebases.