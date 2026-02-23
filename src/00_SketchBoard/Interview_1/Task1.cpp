/*
Task 1: Static Processor Framework
Goal: To assess your understanding of template meta programming for creating flexible, zero-overhead abstractions.

Requirements:
1. Create a CRTP base class template, Processor<Derived>. It should provide a public process(int value) method.
2. The process(int) method must delegate its work to the Derived class:
    - If the Derived class has a public method handle(int), the process(int) call should be forwarded to it.
    - If handle(int) does not exist, the integer must be stored in an internal, fixed-capacity container as a fallback.
3. Implement two derived classes:
    - EchoProcessor: Implements a handle(int) method that records all integers it processes.
    - SilentProcessor: Does not implement handle(int), thereby using the base class's fallback mechanism.
4. In main(), demonstrate that your implementation works correctly. 
Instantiate both processors and show that they behave as expected. 
Ensure that any shared data is accessed in a thread-safe manner when demonstrating your solution.

What we're looking for: A clean CRTP implementation, effective use of 
compile-time logic to create different behaviors, and thread-safety practices.
*/

#include <iostream>
#include <mutex>
#include <concepts>
#include <array>
#include <thread>

/**
 * I have tried to explain everything via the comments and self-describing variable names or such. 
 * I have created a non templated base class to prevent code bloat for the fallback storage and thread safety.
 * The concept is used to check if the handle(int) method exists in the class that inherits from the ProcessorBase class.
 * The CRTP class that inherits from it to implement the processing logic, 
 * which in turn uses the concept to check if the handle(int) method exists and handle it via the appropriate class.
 * I have added #REQ statements to track/map the requirements.
 * */

// I dont like magic numbers
constexpr size_t MAX_NO_OF_VALUES_TO_PROCESS = 5;
constexpr size_t MAX_FALLBACK_SIZE = 100;
constexpr int HANDLE_VALUE = 0xDEADBEEF;

// This is not to bloat the Processor class with thread-safety code, 
// but to centralize it in one place.
class ThreadSafeProcessorBase {
protected:
    // This is the fallback storage mechanism for processors that do not implement handle(int)
    // I dont keep possible complex logic generated for each template
    // and delegate it to ProcessorBase (single copy)
    void storeFallback(int value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_fallback_count < MAX_FALLBACK_SIZE) {
            m_fallback_buffer[m_fallback_count++] = value;
        }
    }

public:
    // For testing purpose, to verify fallback storage
    const size_t getNumberOfFallbackData() const noexcept { return m_fallback_count; }

private:
    // Fixed-capacity buffer to store values for processors without handle()
    std::array<int, MAX_FALLBACK_SIZE> m_fallback_buffer{};
    size_t m_fallback_count{0}; // Track actual number of elements stored
    std::mutex m_mutex; // Mutex to protect fallback buffer access
};

// Concept to check if handle(int) exists
template<typename Class>
concept HasHandle = requires(Class object, int value) {
    // Return type doesn't strictly matter here, but usually void
    { object.handle(value) }; 
};

// CRTP Class - inherits from non-template base
template <typename Derived>
class Processor : public ThreadSafeProcessorBase {
public:
    void process(int value) {
        if constexpr (HasHandle<Derived>) {
            // Derived has a handle(int) function, this code is generated
            static_cast<Derived*>(this)->handle(value);
        } else {
            // Derived does not have a handle(int) function
            storeFallback(value);
        }
    }
};

// #REQ 3.a EchoProcessor
class EchoProcessor : public Processor<EchoProcessor> {
public:
    // Satisfies the CRTP code that will trigger the compiler 
    // genertion of the handle function call
    void handle(int value) noexcept {
        std::cout << "[Echo] Processed: " << value << "\n";
    }
};

// #REQ 3.b SilentProcessor
class SilentProcessor : public Processor<SilentProcessor> {
    // For test purpose i have omit the handle function
};

// REQ 4: Demo
int main() {
    EchoProcessor echo;
    SilentProcessor silent;

    // Lambda to process values in a separate thread
    auto processValues = [&silent](int start, int end) {
        for(int i = start; i < end; ++i)
        silent.process(i);
    };
    
    // Lets test the thread safety of fallback storage by processing values in a separate thread
    std::thread t1(processValues, 0, MAX_NO_OF_VALUES_TO_PROCESS);
    
    echo.process(HANDLE_VALUE); // This will call EchoProcessor::handle and print the value
    t1.join(); // Wait for the thread to finish processing

    std::cout << "No of Silent fallbacks: " << silent.getNumberOfFallbackData() << "\n";
}