# RAII (Resource Acquisition Is Initialization)

## Overview

RAII is a C++ programming idiom that ties resource management to object lifetime. Resources are acquired during object construction and released during object destruction, ensuring that resources are properly cleaned up even in the presence of exceptions.

## Key Benefits

- **Automatic Resource Management**: No need to manually release resources
- **Exception Safety**: Resources are released even if an exception occurs
- **Cleaner Code**: Reduces boilerplate code for resource management
- **Prevents Resource Leaks**: Guarantees cleanup

## Real-World Example: File Handle Management

In this example, we create a `File` class that demonstrates RAII by automatically opening and closing a file. This ensures that the file is always closed, preventing resource leaks.

### Code Example

See `RAIISample.cpp` for the implementation.

### Usage

```cpp
{
    File myFile("example.txt", "w");
    // File is open here
    myFile.write("Hello, RAII!");
    // File is automatically closed when myFile goes out of scope
}
```

This pattern is commonly used for:
- File handles
- Network sockets
- Database connections
- Mutex locks
- Memory management (smart pointers)