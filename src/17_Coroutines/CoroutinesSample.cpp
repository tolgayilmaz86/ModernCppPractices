#include "CoroutinesSample.hpp"
#include <iostream>
#include <coroutine>
#include <optional>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <queue>

// ============================================================================
// C++20 Coroutines: Stackless, Suspendable Functions
// ============================================================================
// Coroutines are functions that can suspend execution and be resumed later.
// Key keywords: co_await, co_yield, co_return
//
// A coroutine requires:
// 1. A promise_type that controls coroutine behavior
// 2. A return type that wraps the promise
// 3. Use of co_await, co_yield, or co_return in the function body

// ============================================================================
// Part 1: Generator - Lazy Sequence Production
// ============================================================================

template <typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Generator(handle_type h) : handle_(h) {}
    
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    
    Generator(Generator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }
    
    ~Generator() {
        if (handle_) handle_.destroy();
    }

    // Iterator support for range-based for loops
    class iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() = default;
        explicit iterator(handle_type handle) : handle_(handle) {}

        iterator& operator++() {
            handle_.resume();
            if (handle_.done()) handle_ = nullptr;
            return *this;
        }

        T& operator*() { return handle_.promise().current_value; }
        
        bool operator==(const iterator& other) const {
            return handle_ == other.handle_;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:
        handle_type handle_ = nullptr;
    };

    iterator begin() {
        if (handle_) {
            handle_.resume();
            if (handle_.done()) return end();
        }
        return iterator{handle_};
    }

    iterator end() { return iterator{}; }

    // Manual iteration
    bool next() {
        if (handle_ && !handle_.done()) {
            handle_.resume();
            return !handle_.done();
        }
        return false;
    }

    T& value() { return handle_.promise().current_value; }

private:
    handle_type handle_;
};

// Generator coroutines using co_yield
Generator<int> range(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

Generator<int> fibonacci(int count) {
    int a = 0, b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

Generator<std::string> split(std::string_view str, char delimiter) {
    std::string current;
    for (char c : str) {
        if (c == delimiter) {
            if (!current.empty()) {
                co_yield current;
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        co_yield current;
    }
}

// ============================================================================
// Part 2: Task - Asynchronous Operations
// ============================================================================

template <typename T>
class Task {
public:
    struct promise_type {
        std::optional<T> result;
        std::exception_ptr exception;
        
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_value(T value) {
            result = std::move(value);
        }
        
        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Task(handle_type h) : handle_(h) {}
    
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    
    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    ~Task() {
        if (handle_) handle_.destroy();
    }

    T get() {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
        return *handle_.promise().result;
    }

    bool done() const { return handle_.done(); }

private:
    handle_type handle_;
};

// Specialization for void
template <>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception;
        
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_void() {}
        
        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Task(handle_type h) : handle_(h) {}
    
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    
    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    ~Task() {
        if (handle_) handle_.destroy();
    }

    void get() {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
    }

    bool done() const { return handle_.done(); }

private:
    handle_type handle_;
};

// Simple async computation
Task<int> compute_async(int x) {
    co_return x * x;
}

Task<std::string> process_string_async(std::string input) {
    std::string result;
    for (char c : input) {
        result += static_cast<char>(std::toupper(c));
    }
    co_return result;
}

// ============================================================================
// Part 3: Awaitable - Custom Suspension Points
// ============================================================================

// A simple awaitable that simulates async delay
struct SimulatedDelay {
    int milliseconds;
    
    bool await_ready() const noexcept { 
        return milliseconds <= 0; 
    }
    
    void await_suspend(std::coroutine_handle<> h) const {
        // In real code, this would schedule resumption
        // For demo, we just print
        std::cout << "    Simulating " << milliseconds << "ms delay..." << std::endl;
    }
    
    void await_resume() const noexcept {}
};

// Awaitable that transforms a value
template <typename T>
struct TransformAwaitable {
    T value;
    std::function<T(T)> transform;
    
    bool await_ready() const noexcept { return false; }
    
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    
    T await_resume() const { 
        return transform(value); 
    }
};

Task<void> demonstrate_awaitable() {
    std::cout << "    Starting awaitable demonstration..." << std::endl;
    
    co_await SimulatedDelay{100};
    std::cout << "    After first delay" << std::endl;
    
    co_await SimulatedDelay{50};
    std::cout << "    After second delay" << std::endl;
    
    co_return;
}

// ============================================================================
// Part 4: Symmetric Transfer
// ============================================================================

// Demonstrates coroutine-to-coroutine transfer without stack growth
struct SymmetricTask {
    struct promise_type {
        std::coroutine_handle<> continuation = std::noop_coroutine();
        
        SymmetricTask get_return_object() {
            return SymmetricTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() noexcept { return {}; }
        
        auto final_suspend() noexcept {
            struct FinalAwaiter {
                bool await_ready() noexcept { return false; }
                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                    return h.promise().continuation;
                }
                void await_resume() noexcept {}
            };
            return FinalAwaiter{};
        }
        
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit SymmetricTask(handle_type h) : handle_(h) {}
    
    SymmetricTask(SymmetricTask&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    ~SymmetricTask() {
        if (handle_) handle_.destroy();
    }

    void start() {
        if (handle_) handle_.resume();
    }

private:
    handle_type handle_;
};

// ============================================================================
// Demonstration Functions
// ============================================================================

void demonstrate_generators() {
    std::cout << "\n=== Generator Coroutines ===" << std::endl;
    
    // Range generator
    std::cout << "Range [1, 6): ";
    for (int i : range(1, 6)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // Fibonacci generator
    std::cout << "Fibonacci (10 numbers): ";
    for (int f : fibonacci(10)) {
        std::cout << f << " ";
    }
    std::cout << std::endl;
    
    // String splitting generator
    std::cout << "Split 'hello,world,coroutines': ";
    for (const auto& word : split("hello,world,coroutines", ',')) {
        std::cout << "[" << word << "] ";
    }
    std::cout << std::endl;
    
    // Lazy evaluation demonstration
    std::cout << "\nLazy evaluation - only compute what's needed:" << std::endl;
    auto gen = fibonacci(1000000);  // Would be expensive if eager
    std::cout << "First 5 fibonacci: ";
    int count = 0;
    for (int f : gen) {
        std::cout << f << " ";
        if (++count >= 5) break;  // Only compute 5
    }
    std::cout << "(stopped early)" << std::endl;
}

void demonstrate_tasks() {
    std::cout << "\n=== Task Coroutines ===" << std::endl;
    
    // Simple async computation
    auto task1 = compute_async(7);
    std::cout << "compute_async(7) = " << task1.get() << std::endl;
    
    auto task2 = process_string_async("hello coroutines");
    std::cout << "process_string_async('hello coroutines') = " << task2.get() << std::endl;
}

void demonstrate_awaitables() {
    std::cout << "\n=== Custom Awaitables ===" << std::endl;
    auto task = demonstrate_awaitable();
    std::cout << "Task completed" << std::endl;
}

void demonstrate_coroutine_concepts() {
    std::cout << "\n=== Coroutine Key Concepts ===" << std::endl;
    
    std::cout << "1. co_yield - Suspend and produce a value (generators)" << std::endl;
    std::cout << "2. co_await - Suspend until awaitable completes" << std::endl;
    std::cout << "3. co_return - Complete coroutine with optional value" << std::endl;
    
    std::cout << "\nPromise type requirements:" << std::endl;
    std::cout << "- get_return_object() - Creates the coroutine return type" << std::endl;
    std::cout << "- initial_suspend() - Controls start behavior" << std::endl;
    std::cout << "- final_suspend() - Controls cleanup behavior" << std::endl;
    std::cout << "- return_void()/return_value() - Handle co_return" << std::endl;
    std::cout << "- unhandled_exception() - Handle exceptions" << std::endl;
    
    std::cout << "\nAwaitable requirements:" << std::endl;
    std::cout << "- await_ready() - Can we skip suspension?" << std::endl;
    std::cout << "- await_suspend() - What to do when suspending" << std::endl;
    std::cout << "- await_resume() - What to return when resuming" << std::endl;
}

void demonstrate_use_cases() {
    std::cout << "\n=== Coroutine Use Cases ===" << std::endl;
    
    std::cout << "1. Generators (lazy sequences)" << std::endl;
    std::cout << "   - Infinite sequences" << std::endl;
    std::cout << "   - Memory-efficient iteration" << std::endl;
    std::cout << "   - On-demand computation" << std::endl;
    
    std::cout << "\n2. Async I/O" << std::endl;
    std::cout << "   - Non-blocking file operations" << std::endl;
    std::cout << "   - Network requests" << std::endl;
    std::cout << "   - Database queries" << std::endl;
    
    std::cout << "\n3. State machines" << std::endl;
    std::cout << "   - Parser implementations" << std::endl;
    std::cout << "   - Protocol handlers" << std::endl;
    std::cout << "   - Game logic" << std::endl;
    
    std::cout << "\n4. Cooperative multitasking" << std::endl;
    std::cout << "   - Single-threaded concurrency" << std::endl;
    std::cout << "   - Event loops" << std::endl;
    std::cout << "   - Task schedulers" << std::endl;
}

void demonstrate_best_practices() {
    std::cout << "\n=== Coroutine Best Practices ===" << std::endl;
    
    std::cout << "DO:" << std::endl;
    std::cout << "- Use coroutines for lazy evaluation" << std::endl;
    std::cout << "- Prefer existing libraries (cppcoro, folly)" << std::endl;
    std::cout << "- Keep promise types simple" << std::endl;
    std::cout << "- Use RAII in coroutine bodies" << std::endl;
    std::cout << "- Consider symmetric transfer for chains" << std::endl;
    
    std::cout << "\nDON'T:" << std::endl;
    std::cout << "- Store references to coroutine locals" << std::endl;
    std::cout << "- Forget to handle exceptions" << std::endl;
    std::cout << "- Ignore coroutine lifetime issues" << std::endl;
    std::cout << "- Use coroutines for simple synchronous code" << std::endl;
    
    std::cout << "\nLibrary recommendations:" << std::endl;
    std::cout << "- cppcoro: General-purpose coroutine library" << std::endl;
    std::cout << "- folly::coro: Facebook's production library" << std::endl;
    std::cout << "- asio: Networking with coroutine support" << std::endl;
}

#include "SampleRegistry.hpp"

void CoroutinesSample::run() {
    std::cout << "Running C++20 Coroutines Sample..." << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Coroutines are stackless, suspendable functions." << std::endl;
    std::cout << "They enable efficient async programming and lazy evaluation." << std::endl;

    demonstrate_generators();
    demonstrate_tasks();
    demonstrate_awaitables();
    demonstrate_coroutine_concepts();
    demonstrate_use_cases();
    demonstrate_best_practices();

    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "1. Coroutines suspend/resume without blocking threads" << std::endl;
    std::cout << "2. Generators produce values lazily with co_yield" << std::endl;
    std::cout << "3. Tasks represent async operations with co_return" << std::endl;
    std::cout << "4. Custom awaitables control suspension with co_await" << std::endl;
    std::cout << "5. Promise types define coroutine behavior" << std::endl;
    std::cout << "6. Use libraries for production async code" << std::endl;

    std::cout << "\nC++20 Coroutines demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(CoroutinesSample, "C++20 Coroutines", 17);
