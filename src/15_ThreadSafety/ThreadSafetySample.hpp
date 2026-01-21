#pragma once

#include "../Testable.hpp"
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>

class ThreadSafetySample : public Testable {
public:
    void run() override;
    std::string name() const override { return "Thread Safety"; }

private:
    // Data Race Demonstration
    void demonstrate_data_race();
    void demonstrate_mutex_solution();
    void demonstrate_atomic_solution();

    // Deadlock Prevention
    void demonstrate_deadlock_risk();
    void demonstrate_scoped_lock_solution();

    // Condition Variables
    void demonstrate_condition_variables();

    // std::async and Futures
    void demonstrate_async_futures();

    // Advanced Patterns
    void demonstrate_thread_safe_queue();
    void demonstrate_reader_writer_lock();

    // Best Practices
    void demonstrate_thread_safety_best_practices();

    // Helper classes
    class BankAccount;
    class ThreadSafeQueue;
    class ReaderWriterLock;
};