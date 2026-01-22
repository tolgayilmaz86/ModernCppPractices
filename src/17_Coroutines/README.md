# C++20 Coroutines

## Overview

Coroutines are **stackless, suspendable functions** introduced in C++20. They can pause execution at specific points and resume later, enabling efficient asynchronous programming without the complexity of threads or callbacks.

**Key Keywords:**
- `co_await` - Suspend until an awaitable completes
- `co_yield` - Suspend and produce a value (for generators)
- `co_return` - Complete the coroutine with an optional value

## When to Use Coroutines

| Use Case | Example |
|----------|---------|
| **Generators** | Lazy sequences, infinite streams, on-demand computation |
| **Async I/O** | Non-blocking file/network operations |
| **State Machines** | Parsers, protocol handlers, game logic |
| **Cooperative Multitasking** | Single-threaded concurrency, event loops |

## Basic Concepts

### 1. Generator Pattern (co_yield)

Generators produce values lazily, only computing the next value when requested:

```cpp
Generator<int> fibonacci(int count) {
    int a = 0, b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;  // Suspend and produce value
        int next = a + b;
        a = b;
        b = next;
    }
}

// Usage - values computed on demand
for (int f : fibonacci(10)) {
    std::cout << f << " ";  // 0 1 1 2 3 5 8 13 21 34
}
```

### 2. Task Pattern (co_return)

Tasks represent asynchronous computations:

```cpp
Task<int> compute_async(int x) {
    // Could await other operations here
    co_return x * x;
}

auto task = compute_async(7);
int result = task.get();  // 49
```

### 3. Awaitable Pattern (co_await)

Custom awaitables control suspension behavior:

```cpp
struct SimulatedDelay {
    int milliseconds;
    
    bool await_ready() const { return milliseconds <= 0; }
    void await_suspend(std::coroutine_handle<> h) const { /* schedule */ }
    void await_resume() const { }
};

Task<void> example() {
    co_await SimulatedDelay{100};  // Suspend here
    std::cout << "Resumed!" << std::endl;
}
```

## Coroutine Components

### Promise Type

Every coroutine needs a `promise_type` that controls its behavior:

```cpp
struct promise_type {
    // Required methods:
    ReturnType get_return_object();     // Create the return type
    auto initial_suspend();              // Suspend at start?
    auto final_suspend() noexcept;       // Cleanup behavior
    void return_void();                  // or return_value(T)
    void unhandled_exception();          // Handle exceptions
    
    // For generators:
    auto yield_value(T value);           // Handle co_yield
};
```

### Awaitable Requirements

Any type can be awaited if it provides:

```cpp
struct Awaitable {
    bool await_ready();                  // Skip suspension?
    void await_suspend(coroutine_handle<>);  // On suspend
    T await_resume();                    // Return value
};
```

## Memory Model

```
┌─────────────────────────────────────────┐
│           Coroutine Frame              │
│  (heap-allocated, compiler-generated)   │
├─────────────────────────────────────────┤
│  - Promise object                       │
│  - Function parameters (copies)         │
│  - Local variables                      │
│  - Suspension point info                │
│  - Temporary values across suspensions  │
└─────────────────────────────────────────┘
```

**Key points:**
- Coroutine frames are typically heap-allocated
- Local variables persist across suspensions
- Parameters are copied into the frame
- Compiler manages frame lifetime

## Coroutines vs Alternatives

| Aspect | Coroutines | Threads | Callbacks |
|--------|------------|---------|-----------|
| **Memory** | Small frame | Large stack | Minimal |
| **Context Switch** | Very cheap | Expensive | None |
| **Code Style** | Sequential | Sequential | Nested/inverted |
| **Complexity** | Medium | High | High |
| **Debugging** | Medium | Hard | Hard |

## Best Practices

### ✅ DO:
- Use coroutines for lazy evaluation and generators
- Prefer established libraries (cppcoro, folly::coro, asio)
- Keep promise types simple and focused
- Use RAII for resources in coroutine bodies
- Consider symmetric transfer for coroutine chains
- Mark `final_suspend` as `noexcept`

### ❌ DON'T:
- Store references to coroutine local variables externally
- Forget to handle exceptions in promise types
- Ignore coroutine lifetime (dangling handles)
- Use coroutines for simple synchronous code
- Resume a destroyed or completed coroutine

## Common Pitfalls

### 1. Dangling References

```cpp
// ❌ BAD: Reference to local becomes dangling
Generator<int&> bad_generator() {
    int x = 42;
    co_yield x;  // x destroyed after suspension!
}

// ✅ GOOD: Return by value
Generator<int> good_generator() {
    int x = 42;
    co_yield x;  // Value is copied
}
```

### 2. Lifetime Issues

```cpp
// ❌ BAD: Coroutine may outlive the string
Task<void> process(const std::string& s) {
    co_await some_async_op();
    use(s);  // s may be destroyed!
}

// ✅ GOOD: Take ownership
Task<void> process(std::string s) {
    co_await some_async_op();
    use(s);  // s is safely in coroutine frame
}
```

## Recommended Libraries

| Library | Description |
|---------|-------------|
| **cppcoro** | General-purpose coroutine primitives |
| **folly::coro** | Facebook's production-ready library |
| **asio** | Networking with coroutine support |
| **libcoro** | Lightweight coroutine library |

## Further Reading

- [cppreference: Coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- [Lewis Baker's Coroutine Theory](https://lewissbaker.github.io/)
- [C++20 Coroutines Proposal](https://wg21.link/p0912)
