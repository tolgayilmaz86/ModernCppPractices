# Thread Safety in Modern C++

## Overview

Writing thread-safe code is essential in modern C++ applications. Thread safety ensures that shared data is accessed correctly when multiple threads execute concurrently. This sample demonstrates best practices for thread safety using modern C++ concurrency features.

**The Problem:** Multiple threads accessing shared data simultaneously can cause data races, crashes, and unpredictable behavior. Traditional locking mechanisms can lead to deadlocks and performance issues.

**The Solution:** Use modern C++ concurrency primitives like mutexes, atomics, condition variables, and futures to write safe, efficient concurrent code.

## How Thread Safety Works

1. **Mutexes:** Mutual exclusion locks prevent simultaneous access to shared data
2. **Atomics:** Lock-free operations for simple data types
3. **Condition Variables:** Efficient waiting and notification between threads
4. **Futures:** Asynchronous task execution and result retrieval
5. **Scoped Locks:** Deadlock-free multiple mutex acquisition

```cpp
// Basic thread safety with mutex
std::mutex mtx;
int shared_data = 0;

void thread_safe_increment() {
    std::lock_guard<std::mutex> lock(mtx);  // RAII lock
    shared_data++;
}

// Lock-free with atomics
std::atomic<int> atomic_counter{0};

void atomic_increment() {
    atomic_counter++;  // No locks needed
}
```

## Sample Output

```
Running Thread Safety Sample...

=== Data Race Demonstration ===
Expected: 200000, Actual: 150843
Data race caused lost updates!

=== Mutex Solution ===
With mutex - Expected: 200000, Actual: 200000
Mutex prevents data races!

=== Atomic Solution ===
With atomic - Expected: 200000, Actual: 200000
Atomics provide lock-free thread safety!
Flag was: false, now: true

=== Scoped Lock Solution (Deadlock-Free) ===
Transferred $200 successfully
Transferred $100 successfully
Final balances - A: $900, B: $600

=== Condition Variables - Producer/Consumer ===
Produced: Message 0
Consumed: Message 0
Produced: Message 1
Consumed: Message 1
Producer-consumer pattern completed!

=== std::async and std::future ===
Tasks started, doing other work...
Starting expensive calculation for 10
Starting expensive calculation for 20
Results: 100, 400
Packaged task result: 225

=== Thread-Safe Queue Implementation ===
P1-M0
C1 got: P1-M0
P2-M0
C2 got: P2-M0
Thread-safe queue demonstration completed!

=== Reader-Writer Lock Pattern ===
Reader active, data: 0 (readers: 1)
Reader active, data: 0 (readers: 2)
Writer updated data to: 1
Reader-writer pattern allows multiple concurrent readers!

=== Thread Safety Best Practices ===
1. Prefer immutable data when possible
2. Use atomics for simple operations
3. Use mutexes for complex operations
4. Always lock mutexes in the same order
5. Use scoped_lock for multiple mutexes
6. Minimize lock duration
7. Consider lock-free alternatives when appropriate
8. Use condition variables for waiting
9. Avoid busy waiting
10. Test with thread sanitizer (TSAN)

RAII with locks (automatic unlock):
Lock acquired
Lock automatically released

Lock hierarchy prevents deadlocks:
- Always acquire locks in the same order
- Use a global lock ordering
- Consider lock levels or addresses

Thread Safety demonstration completed!
```

## Key Components

### 1. Data Races and Race Conditions
- **Data Race:** Undefined behavior when multiple threads access shared memory
- **Race Condition:** Program behavior depends on thread execution timing
- **Detection:** Use thread sanitizer (TSAN) or careful code review

### 2. Mutexes and Locks
- **std::mutex:** Basic mutual exclusion
- **std::lock_guard:** RAII wrapper for automatic unlock
- **std::unique_lock:** More flexible locking with manual control
- **std::scoped_lock:** Deadlock-free multiple mutex locking

### 3. Atomic Operations
- **std::atomic:** Lock-free operations on fundamental types
- **Memory Ordering:** Control when atomic operations become visible
- **Performance:** Faster than mutexes for simple operations

### 4. Condition Variables
- **std::condition_variable:** Wait/notify mechanism between threads
- **Producer-Consumer:** Classic pattern for thread communication
- **Spurious Wakeups:** Always check condition after waking

### 5. Futures and Async
- **std::future:** Represents asynchronous operation result
- **std::async:** Launch asynchronous tasks
- **std::packaged_task:** Wrap callable objects for async execution

## Implementation Details

This sample demonstrates:

1. **Data Race Demonstration:** Shows problems with unsynchronized access
2. **Mutex Solutions:** RAII locking with lock_guard
3. **Atomic Solutions:** Lock-free operations with std::atomic
4. **Deadlock Prevention:** Scoped locks and proper lock ordering
5. **Condition Variables:** Producer-consumer pattern implementation
6. **Async Futures:** Asynchronous task execution
7. **Thread-Safe Queue:** Custom thread-safe container
8. **Reader-Writer Locks:** Multiple readers, exclusive writers
9. **Best Practices:** Guidelines for writing thread-safe code

## When to Use Each Concurrency Primitive

### ✅ **Use std::mutex + lock_guard when:**
- Protecting complex data structures
- Operations take significant time
- Need to maintain invariants across multiple variables

### ✅ **Use std::atomic when:**
- Simple operations on fundamental types
- High-performance requirements
- Lock-free algorithms

### ✅ **Use std::scoped_lock when:**
- Acquiring multiple mutexes simultaneously
- Preventing deadlocks
- Need atomic multi-lock operations

### ✅ **Use std::condition_variable when:**
- Producer-consumer patterns
- Waiting for specific conditions
- Efficient thread communication

### ✅ **Use std::async + std::future when:**
- Running tasks in parallel
- Getting results from background work
- Simple parallel execution

## Performance Characteristics

| Primitive | Performance | Use Case | Thread Safety |
|-----------|-------------|----------|----------------|
| std::atomic | Very Fast | Simple counters/flags | Lock-free |
| std::mutex | Medium | Complex operations | Exclusive access |
| std::shared_mutex | Medium | Reader-writer | Multiple readers |
| std::condition_variable | Fast | Waiting/notification | Coordination |
| std::async | Variable | Parallel tasks | Result synchronization |

**Performance Tips:**
- Use atomics for simple operations
- Minimize lock duration
- Prefer reader-writer locks for read-heavy workloads
- Avoid frequent locking/unlocking
- Consider lock-free algorithms for high contention

## Common Thread Safety Patterns

### 1. Thread-Safe Singleton
```cpp
class Singleton {
public:
    static Singleton& instance() {
        static Singleton instance;  // Thread-safe in C++11+
        return instance;
    }
private:
    Singleton() = default;
};
```

### 2. Thread-Safe Lazy Initialization
```cpp
class LazyResource {
private:
    mutable std::mutex mutex_;
    mutable std::unique_ptr<Resource> resource_;
    mutable std::once_flag flag_;

public:
    const Resource& get_resource() const {
        std::call_once(flag_, [this]() {
            resource_ = std::make_unique<Resource>();
        });
        return *resource_;
    }
};
```

### 3. Monitor Object Pattern
```cpp
class Monitor {
private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    // Shared state here

public:
    void wait_for_condition() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return condition_met(); });
    }

    void notify_condition() {
        std::lock_guard<std::mutex> lock(mutex_);
        // Update condition
        cv_.notify_all();
    }
};
```

## Avoiding Common Pitfalls

### 1. Deadlocks
```cpp
// ❌ Wrong: Different lock orders
void transfer(Account& a, Account& b) {
    std::lock_guard lock_a(a.mutex);
    std::lock_guard lock_b(b.mutex);  // May deadlock
}

// ✅ Right: Consistent lock ordering
void transfer(Account& a, Account& b) {
    std::scoped_lock lock(a.mutex, b.mutex);  // Atomic
}
```

### 2. Race Conditions in Condition Variables
```cpp
// ❌ Wrong: Check-then-wait race
if (!condition) {
    cv.wait(lock);  // Condition might change here
}

// ✅ Right: Wait with predicate
cv.wait(lock, [&]() { return condition; });
```

### 3. Forgotten Unlocks
```cpp
// ❌ Wrong: Manual lock/unlock
mutex.lock();
// ... do work
if (error) return;  // Forgot to unlock!
// mutex.unlock();

// ✅ Right: RAII
std::lock_guard lock(mutex);
// Automatic unlock on scope exit
```

## Modern C++ Concurrency Features

### C++11 Features
- `std::thread`: Basic thread management
- `std::mutex`, `std::lock_guard`: Basic locking
- `std::atomic`: Lock-free operations
- `std::condition_variable`: Thread synchronization
- `std::future`, `std::async`: Asynchronous operations

### C++14 Features
- `std::shared_mutex`: Reader-writer locks
- Improved `std::async` behavior

### C++17 Features
- `std::scoped_lock`: Deadlock-free multi-locking
- Parallel algorithms in `<algorithm>` and `<numeric>`
- `std::shared_mutex` improvements

### C++20 Features
- Semaphores: `std::counting_semaphore`, `std::binary_semaphore`
- Latches and barriers: `std::latch`, `std::barrier`
- Coroutines: `co_await`, `co_yield`, `co_return`
- `std::atomic<std::shared_ptr>`: Atomic shared pointers

## Testing Thread-Safe Code

### 1. Unit Testing with Threads
```cpp
TEST(ThreadSafety, ConcurrentCounter) {
    std::atomic<int> counter{0};

    auto increment = [&counter]() {
        for (int i = 0; i < 1000; i++) {
            counter++;
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();

    EXPECT_EQ(counter.load(), 2000);
}
```

### 2. Thread Sanitizer (TSAN)
```bash
# Compile with thread sanitizer
g++ -fsanitize=thread -g main.cpp -o main
# Run to detect data races
./main
```

### 3. Stress Testing
```cpp
TEST(ThreadSafety, StressTest) {
    const int num_threads = 10;
    const int iterations = 10000;

    // Run multiple threads with high contention
    // Verify no data races or deadlocks
}
```

## Best Practices Summary

1. **Design for Concurrency:** Consider threading from the start
2. **Prefer Immutability:** Immutable data is inherently thread-safe
3. **Use RAII:** Automatic resource management prevents leaks
4. **Lock Hierarchies:** Define global lock ordering to prevent deadlocks
5. **Minimize Lock Scope:** Hold locks for minimum time possible
6. **Test Thoroughly:** Use TSAN and stress testing
7. **Document Thread Safety:** Clearly mark thread-safe guarantees
8. **Avoid Global State:** Global variables complicate threading
9. **Use High-Level Primitives:** Prefer `std::async` over raw threads
10. **Profile Performance:** Measure contention and optimize bottlenecks

Thread safety is not just about using the right primitives—it's about designing systems that minimize shared mutable state and maximize concurrency. Modern C++ provides excellent tools for this, but they require careful application and thorough testing.

---