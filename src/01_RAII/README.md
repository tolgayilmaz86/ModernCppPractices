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

## Real-World Examples

See `RAIISample.cpp` for the implementations.

### Example 1: File Handle Management

In this example, we create a `File` class that demonstrates RAII by automatically opening and closing a file. This ensures that the file is always closed, preventing resource leaks.

```cpp
{
    File myFile("example.txt", "w");
    // File is open here
    myFile.write("Hello, RAII!");
    // File is automatically closed when myFile goes out of scope
}
```

### Example 2: ScopedTimer (Execution Time Measurement)

This example demonstrates RAII for automatic timing - a practical pattern for profiling and debugging. The timer starts when constructed and automatically reports the elapsed time when it goes out of scope.

```cpp
// ✅ GOOD: RAII timer - no need to manually stop or calculate duration
class ScopedTimer {
    std::string operationName;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
public:
    explicit ScopedTimer(const std::string& name)
        : operationName(name), startTime(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << operationName << " completed in " << duration.count() << " ms\n";
    }

    // Non-copyable, non-movable
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};

// Usage
{
    ScopedTimer timer("Database query");
    performDatabaseQuery();  // If this throws, timer still reports!
}   // Automatically prints: "Database query completed in X ms"
```

**Key Insight:** Nested timers work naturally with RAII - inner timers report first (LIFO order), just like a call stack:

```cpp
{
    ScopedTimer outer("Outer");
    {
        ScopedTimer inner("Inner");
        // ... work ...
    }   // Inner reports first
}   // Outer reports second
```

### Example 3: Wrap a legacy C driver (raw pointers) in Modern C++20 safely using RAII and std::span

Say that we have to wrap a legacy C driver that expects a uint8_t* buffer, int len callback in Modern C++20? 
Specifically, we use std::span and RAII to ensure that even if the C driver acts weird, our C++ application layer remains memory-safe.

What we need is:
- Usage of std::span to wrap the raw C pointers safely.
- Usage of a wrapper class (RAII) that handles the C driver's init/deinit.

We would create an RAII Wrapper Class for the C driver:
1. Constructor: Calls the C driver's init() function.
2. Destructor: Calls deinit(), ensuring resources are freed even if an exception occurs (Exception Safety).
3. Interface: Replace uint8_t* buf, int len with std::span<uint8_t>.

> Note: std::span is a non-owning view over a contiguous sequence of objects. It does not own the memory it points to.

```cpp
// C-style driver interface
typedef void* DeviceHandle;

int c_driver_init(DeviceHandle* handle);
int c_driver_process(DeviceHandle handle, const uint8_t* buffer, int len);
int c_driver_deinit(DeviceHandle handle);

// Modern C++20 RAII Wrapper
class DeviceWrapper {
    DeviceHandle handle = nullptr;
public:
    DeviceWrapper() {
        if (c_driver_init(&handle)) throw std::runtime_error("Failed to initialize C driver");
    }
    ~DeviceWrapper() { if (handle) c_driver_deinit(handle); }

    // Process data using std::span
    int process(std::span<uint8_t> buffer) {
        if (!handle) return -1;
        return c_driver_process(handle, buffer.data(), static_cast<int>(buffer.size()));
    }
};

// Usage in Modern C++20
void processData(std::vector<uint8_t>& data) {
    DeviceWrapper device;
    
    // Use std::span to wrap the vector's data
    std::span<uint8_t> buffer(data.data(), data.size());
    
    int result = device.process(buffer);
    if (result != 0) {
        throw std::runtime_error("C driver processing failed");
    }
}
```

---

This pattern is commonly used for:
- File handles
- Network sockets
- Database connections
- Mutex locks
- Memory management (smart pointers)
- Execution timing and profiling
- Scope guards and cleanup actions
- Wrapping legacy C APIs