# RAII (Resource Acquisition Is Initialization)

## Overview

RAII is a C++ programming idiom that ties resource management to object lifetime. Resources are acquired during object construction and released during object destruction, ensuring that resources are properly cleaned up even in the presence of exceptions.

**The Problem:**
Resources (memory, files, sockets, mutexes) need to be released after use. Forgetting to release them causes leaks. Exception paths make manual cleanup even harder.

```cpp
// ❌ BAD: Manual resource management - easy to leak!
void processFile(const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    if (!file) return;  // OK
    
    try {
        doSomething();     // What if this throws?
        doSomethingElse(); // Or this?
    } catch (...) {
        fclose(file);      // Must remember to close here!
        throw;
    }
    
    fclose(file);  // And here! Easy to forget.
}
```

**The Solution: RAII**
Tie the resource lifetime to an object's lifetime. Acquire in constructor, release in destructor. C++ guarantees destructors run when objects go out of scope - even during exceptions!

```cpp
// ✅ GOOD: RAII wrapper handles cleanup automatically
class FileHandle {
    FILE* file;
public:
    explicit FileHandle(const std::string& path) 
        : file(fopen(path.c_str(), "r")) 
    {
        if (!file) throw std::runtime_error("Cannot open file");
    }
    
    ~FileHandle() { 
        if (file) fclose(file);  // Always called, even during exceptions!
    }
    
    // Prevent copying (or implement deep copy)
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    
    // Allow moving
    FileHandle(FileHandle&& other) noexcept : file(other.file) {
        other.file = nullptr;
    }
    
    FILE* get() { return file; }
};

void processFile(const std::string& path) {
    FileHandle file(path);  // Resource acquired
    doSomething();          // If this throws...
    doSomethingElse();      // Or this...
}   // file destructor ALWAYS runs - no leak possible!
```

**RAII in the Standard Library:**
| Resource | RAII Wrapper |
|----------|--------------|
| Heap memory | `std::unique_ptr`, `std::shared_ptr` |
| Arrays | `std::vector`, `std::array` |
| Strings | `std::string` |
| Files | `std::fstream`, `std::ifstream`, `std::ofstream` |
| Mutexes | `std::lock_guard`, `std::unique_lock`, `std::scoped_lock` |
| Threads | `std::jthread` (C++20) |

**Key Insight:** If you're writing `new`/`delete`, `malloc`/`free`, `fopen`/`fclose`, or any acquire/release pair manually, you probably need an RAII wrapper instead!

---

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