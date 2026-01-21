#include "ThreadSafetySample.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>
#include <chrono>
#include <shared_mutex>

// ============================================================================
// Helper Classes for Demonstrations
// ============================================================================

class ThreadSafetySample::BankAccount {
private:
    double balance_ = 0.0;
    mutable std::mutex mutex_;

public:
    BankAccount(double initial_balance = 0.0) : balance_(initial_balance) {}

    void deposit(double amount) {
        std::lock_guard<std::mutex> lock(mutex_);
        balance_ += amount;
        std::cout << "Deposited $" << amount << ", new balance: $" << balance_ << std::endl;
    }

    void withdraw(double amount) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (balance_ >= amount) {
            balance_ -= amount;
            std::cout << "Withdrew $" << amount << ", new balance: $" << balance_ << std::endl;
        } else {
            std::cout << "Insufficient funds for withdrawal of $" << amount << std::endl;
        }
    }

    double get_balance() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return balance_;
    }

    // Thread-safe transfer between accounts
    static void transfer(BankAccount& from, BankAccount& to, double amount) {
        std::scoped_lock lock(from.mutex_, to.mutex_);  // Locks both atomically
        if (from.balance_ >= amount) {
            from.balance_ -= amount;
            to.balance_ += amount;
            std::cout << "Transferred $" << amount << " successfully" << std::endl;
        } else {
            std::cout << "Transfer failed: insufficient funds" << std::endl;
        }
    }
};

class ThreadSafetySample::ThreadSafeQueue {
private:
    std::queue<std::string> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;

public:
    void push(std::string value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value));
        }
        cv_.notify_one();
    }

    std::string pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });

        std::string value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    bool try_pop(std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

class ThreadSafetySample::ReaderWriterLock {
private:
    mutable std::shared_mutex mutex_;

public:
    void read_lock() const { mutex_.lock_shared(); }
    void read_unlock() const { mutex_.unlock_shared(); }
    void write_lock() { mutex_.lock(); }
    void write_unlock() { mutex_.unlock(); }
};

// ============================================================================
// Implementation
// ============================================================================

void ThreadSafetySample::run() {
    std::cout << "Running Thread Safety Sample..." << std::endl;

    demonstrate_data_race();
    demonstrate_mutex_solution();
    demonstrate_atomic_solution();
    demonstrate_deadlock_risk();
    demonstrate_scoped_lock_solution();
    demonstrate_condition_variables();
    demonstrate_async_futures();
    demonstrate_thread_safe_queue();
    demonstrate_reader_writer_lock();
    demonstrate_thread_safety_best_practices();

    std::cout << "\nThread Safety demonstration completed!" << std::endl;
}

void ThreadSafetySample::demonstrate_data_race() {
    std::cout << "\n=== Data Race Demonstration ===" << std::endl;

    int counter = 0;
    auto increment = [&counter]() {
        for (int i = 0; i < 100000; i++) {
            counter++;  // Data race here!
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Expected: 200000, Actual: " << counter << std::endl;
    std::cout << "Data race caused lost updates!" << std::endl;
}

void ThreadSafetySample::demonstrate_mutex_solution() {
    std::cout << "\n=== Mutex Solution ===" << std::endl;

    int counter = 0;
    std::mutex counter_mutex;

    auto safe_increment = [&counter, &counter_mutex]() {
        for (int i = 0; i < 100000; i++) {
            std::lock_guard<std::mutex> lock(counter_mutex);
            counter++;
        }
    };

    std::thread t1(safe_increment);
    std::thread t2(safe_increment);

    t1.join();
    t2.join();

    std::cout << "With mutex - Expected: 200000, Actual: " << counter << std::endl;
    std::cout << "Mutex prevents data races!" << std::endl;
}

void ThreadSafetySample::demonstrate_atomic_solution() {
    std::cout << "\n=== Atomic Solution ===" << std::endl;

    std::atomic<int> counter{0};

    auto atomic_increment = [&counter]() {
        for (int i = 0; i < 100000; i++) {
            counter++;  // Atomic operation
        }
    };

    std::thread t1(atomic_increment);
    std::thread t2(atomic_increment);

    t1.join();
    t2.join();

    std::cout << "With atomic - Expected: 200000, Actual: " << counter.load() << std::endl;
    std::cout << "Atomics provide lock-free thread safety!" << std::endl;

    // Demonstrate other atomic operations
    std::atomic<bool> flag{false};
    std::cout << "Flag was: " << flag.exchange(true) << ", now: " << flag.load() << std::endl;
}

void ThreadSafetySample::demonstrate_deadlock_risk() {
    std::cout << "\n=== Deadlock Risk Demonstration (Conceptual) ===" << std::endl;

    // NOTE: We don't actually run deadlocking code - it would hang the sample!
    // Instead, we demonstrate the concept:

    std::cout << "Deadlock occurs when two threads each hold a lock the other needs:" << std::endl;
    std::cout << std::endl;
    std::cout << "// DANGEROUS CODE (would deadlock):" << std::endl;
    std::cout << "// Thread 1:                    Thread 2:" << std::endl;
    std::cout << "// lock(mutex_a);               lock(mutex_b);" << std::endl;
    std::cout << "// lock(mutex_b); // waits...   lock(mutex_a); // waits..." << std::endl;
    std::cout << "// Both threads wait forever - DEADLOCK!" << std::endl;
    std::cout << std::endl;

    // Safe demonstration - same lock order
    std::mutex mutex_a, mutex_b;
    int shared_data = 0;

    auto safe_function_1 = [&]() {
        std::lock_guard<std::mutex> lock_a(mutex_a);
        std::lock_guard<std::mutex> lock_b(mutex_b);  // Same order as thread 2
        shared_data++;
        std::cout << "Thread 1 completed (same lock order - safe)" << std::endl;
    };

    auto safe_function_2 = [&]() {
        std::lock_guard<std::mutex> lock_a(mutex_a);  // Same order as thread 1
        std::lock_guard<std::mutex> lock_b(mutex_b);
        shared_data++;
        std::cout << "Thread 2 completed (same lock order - safe)" << std::endl;
    };

    std::thread t1(safe_function_1);
    std::thread t2(safe_function_2);

    t1.join();
    t2.join();

    std::cout << "Shared data: " << shared_data << std::endl;
    std::cout << "Prevention: Always acquire locks in the same order, or use std::scoped_lock!" << std::endl;
}

void ThreadSafetySample::demonstrate_scoped_lock_solution() {
    std::cout << "\n=== Scoped Lock Solution (Deadlock-Free) ===" << std::endl;

    BankAccount account_a(1000.0);
    BankAccount account_b(500.0);

    std::thread t1(BankAccount::transfer, std::ref(account_a), std::ref(account_b), 200.0);
    std::thread t2(BankAccount::transfer, std::ref(account_b), std::ref(account_a), 100.0);

    t1.join();
    t2.join();

    std::cout << "Final balances - A: $" << account_a.get_balance()
              << ", B: $" << account_b.get_balance() << std::endl;
}

void ThreadSafetySample::demonstrate_condition_variables() {
    std::cout << "\n=== Condition Variables - Producer/Consumer ===" << std::endl;

    ThreadSafeQueue queue;
    bool finished = false;

    auto producer = [&]() {
        for (int i = 0; i < 5; i++) {
            std::string msg = "Message " + std::to_string(i);
            queue.push(msg);
            std::cout << "Produced: " << msg << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        finished = true;
        queue.push("DONE");  // Signal completion
    };

    auto consumer = [&]() {
        while (true) {
            std::string msg = queue.pop();
            if (msg == "DONE") break;
            std::cout << "Consumed: " << msg << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

    std::cout << "Producer-consumer pattern completed!" << std::endl;
}

void ThreadSafetySample::demonstrate_async_futures() {
    std::cout << "\n=== std::async and std::future ===" << std::endl;

    auto expensive_calculation = [](int x) -> int {
        std::cout << "Starting expensive calculation for " << x << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return x * x;
    };

    // Start async tasks
    std::future<int> result1 = std::async(std::launch::async, expensive_calculation, 10);
    std::future<int> result2 = std::async(std::launch::async, expensive_calculation, 20);

    std::cout << "Tasks started, doing other work..." << std::endl;

    // Get results (blocks if not ready)
    int val1 = result1.get();
    int val2 = result2.get();

    std::cout << "Results: " << val1 << ", " << val2 << std::endl;

    // Demonstrate packaged_task
    std::packaged_task<int(int)> task(expensive_calculation);
    std::future<int> task_result = task.get_future();

    std::thread task_thread(std::move(task), 15);
    std::cout << "Packaged task result: " << task_result.get() << std::endl;
    task_thread.join();
}

void ThreadSafetySample::demonstrate_thread_safe_queue() {
    std::cout << "\n=== Thread-Safe Queue Implementation ===" << std::endl;

    ThreadSafeQueue queue;

    auto producer = [&queue](int id) {
        for (int i = 0; i < 3; i++) {
            std::string msg = "P" + std::to_string(id) + "-M" + std::to_string(i);
            queue.push(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    };

    auto consumer = [&queue](int id) {
        for (int i = 0; i < 3; i++) {
            std::string msg;
            if (queue.try_pop(msg)) {
                std::cout << "C" << id << " got: " << msg << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
    };

    std::vector<std::thread> threads;
    threads.emplace_back(producer, 1);
    threads.emplace_back(producer, 2);
    threads.emplace_back(consumer, 1);
    threads.emplace_back(consumer, 2);

    for (auto& t : threads) t.join();

    std::cout << "Thread-safe queue demonstration completed!" << std::endl;
}

void ThreadSafetySample::demonstrate_reader_writer_lock() {
    std::cout << "\n=== Reader-Writer Lock Pattern ===" << std::endl;

    ReaderWriterLock rw_lock;
    int shared_data = 0;
    std::atomic<int> readers_active{0};

    auto reader = [&]() {
        rw_lock.read_lock();
        readers_active++;
        std::cout << "Reader active, data: " << shared_data << " (readers: " << readers_active.load() << ")" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        readers_active--;
        rw_lock.read_unlock();
    };

    auto writer = [&]() {
        rw_lock.write_lock();
        shared_data++;
        std::cout << "Writer updated data to: " << shared_data << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        rw_lock.write_unlock();
    };

    std::vector<std::thread> threads;

    // Start multiple readers
    for (int i = 0; i < 3; i++) {
        threads.emplace_back(reader);
    }

    // Start a writer
    threads.emplace_back(writer);

    // More readers
    for (int i = 0; i < 2; i++) {
        threads.emplace_back(reader);
    }

    for (auto& t : threads) t.join();

    std::cout << "Reader-writer pattern allows multiple concurrent readers!" << std::endl;
}

void ThreadSafetySample::demonstrate_thread_safety_best_practices() {
    std::cout << "\n=== Thread Safety Best Practices ===" << std::endl;

    std::cout << "1. Prefer immutable data when possible" << std::endl;
    std::cout << "2. Use atomics for simple operations" << std::endl;
    std::cout << "3. Use mutexes for complex operations" << std::endl;
    std::cout << "4. Always lock mutexes in the same order" << std::endl;
    std::cout << "5. Use scoped_lock for multiple mutexes" << std::endl;
    std::cout << "6. Minimize lock duration" << std::endl;
    std::cout << "7. Consider lock-free alternatives when appropriate" << std::endl;
    std::cout << "8. Use condition variables for waiting" << std::endl;
    std::cout << "9. Avoid busy waiting" << std::endl;
    std::cout << "10. Test with thread sanitizer (TSAN)" << std::endl;

    // Demonstrate RAII with locks
    std::cout << "\nRAII with locks (automatic unlock):" << std::endl;
    std::mutex test_mutex;

    {
        std::lock_guard<std::mutex> lock(test_mutex);
        std::cout << "Lock acquired" << std::endl;
        // Lock automatically released when lock_guard goes out of scope
    }
    std::cout << "Lock automatically released" << std::endl;

    // Demonstrate lock hierarchy
    std::cout << "\nLock hierarchy prevents deadlocks:" << std::endl;
    std::cout << "- Always acquire locks in the same order" << std::endl;
    std::cout << "- Use a global lock ordering" << std::endl;
    std::cout << "- Consider lock levels or addresses" << std::endl;
}