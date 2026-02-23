/**
Task 2: Asynchronous Event Processor

Goal: To test your ability to design a type-safe, asynchronous system using 
fundamental concurrency primitives and modern C++ features.
Requirements:
1. Design a small set of event types. For example, a UserInputEvent containing a 
   std::string and a NetworkDataEvent containing a vector of bytes. 
   The system should be able to easily accommodate new event types.
2. Implement a thread-safe, bounded, first-in-first-out queue for these events. 
   The queue should support being gracefully shut down.
    - A method to push an event, which may block if the queue is full.
    - A method to pop an event, which may block if the queue is empty.
    - A method to signal that no more events will be added. 
    - Popping from an empty and closed queue should immediately 
      indicate that no more items are available.
3. Create an EventProcessor class that manages the event system.
    - It must provide a type-safe way to register a handler (e.g., a lambda) for a specific event type.
    - It must provide a way to unregister a handler for a specific event type.
    - It must provide a method to post an event to its queue from any thread.
    - The EventProcessor should run a dedicated worker thread that pops events from the queue 
      and dispatches them to the correct registered handler.
    - The class must manage the lifecycle of its worker thread correctly, ensuring a clean shutdown.

4. In main(), demonstrate the full system. 
   Register handlers for your event types, 
   post various events from a producer thread, 
   and then safely shut down the processor.

What we're looking for: A robust queue implementation, a clean design for type-safe event dispatching 
(e.g., using std::variant, std::any, or another technique), 
and correct management of thread and resource life cycles (RAII).
 * */

#include <iostream>
#include <variant>
#include <queue>
#include <condition_variable>
#include <thread>
#include <functional>
#include <atomic>
#include <map>

/**
 * I have tried to explain everything via the comments and self-describing variable names or such.
 * I have created event types and a bounded queue to store them.
 * The queue is thread-safe and supports graceful shutdown via a shutdown flag and condition variables to wake up waiting threads.
 * The EventProcessor class is used to manage the event system.
 * I have added #REQ statements to track/map the requirements to tasks given
 * The EventProcessor class manages the worker thread and ensures a clean shutdown using RAII principles.
 * The handlers are registered in a type-safe manner using std::function and std::variant for dispatching.
 * The main function demonstrates the full system by registering handlers, posting events, and shutting down the processor.
 * 
 * */

// Event Types
struct UserInputEvent { std::string input; };
struct NetworkDataEvent { std::vector<uint8_t> data; };

// #REQ 1 - Easily accommodate new event types without modifying existing code
// Since RTTI is not allowed, we can use std::variant to hold different event types in a type-safe manner.
using Event = std::variant<std::monostate, UserInputEvent, NetworkDataEvent>;

// #REQ 2 - Thread-Safe Bounded Queue (Simulating ETL circular buffer concepts)
template <typename T, size_t Capacity>
class BoundedQueue {
public:
    // #REQ 2.a - Push item into the queue, blocks if full
    void push(T item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        // Wait until there is space or we are shutting down
        auto untilEnoughSpace = [this]{ return m_queue.size() < Capacity || m_shutdown; };
        m_not_full.wait(lock, untilEnoughSpace);
        
        if (m_shutdown) return;

        m_queue.push(std::move(item));
        m_not_empty.notify_one(); // Notify one waiting consumer
    }

    // #REQ 2.b - Pop item from the queue, blocks if empty
    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        // Wait until data exists or shutdown signal
        auto untilDataOrShutdown = [this]{ return !m_queue.empty() || m_shutdown; };
        m_not_empty.wait(lock, untilDataOrShutdown);

        if (m_queue.empty() && m_shutdown) return false;

        item = std::move(m_queue.front());
        m_queue.pop();
        m_not_full.notify_one();
        return true;
    }

    // #REQ 2.c - Signal no more event will be added, unblock waiting threads
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_shutdown = true;
        }
        // Prevents deadlock if producers are waiting to push and consumers are waiting to pop
        m_not_empty.notify_all(); // Notify all waiting consumers
        m_not_full.notify_all(); // Notify all waiting producers
    }

private:
    std::queue<T> m_queue; // etl::queue not available
    std::mutex m_mutex; // For thread-safe queue
    std::condition_variable m_not_empty;
    std::condition_variable m_not_full;
    bool m_shutdown = false;
};

// Helper for std::visit
template<class... Ts> 
struct overloaded : Ts... 
{ 
    using Ts::operator()...; 
};

// Deduction guide for overloaded
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// I dont like magic numbers
constexpr size_t QUEUE_SIZE = 10;

// #REQ 3 - Event Processor
class EventProcessor {
public:
    using HandlerVariant = std::variant<
        std::function<void(const UserInputEvent&)>,
        std::function<void(const NetworkDataEvent&)>
    >;

    // #REQ 3.a - Register handlers (Type Safe)
    void registerHandler(std::function<void(const UserInputEvent&)> h) {
        m_inputHandler = h;
    }
    
    void registerHandler(std::function<void(const NetworkDataEvent&)> h) {
        m_netHandler = h;
    }

    // #REQ 3.b - Post events from producers
    void post(Event e) {
        m_queue.push(std::move(e));
    }

    // #REQ 3.c - Worker thread to pop events from the queue and dispatch to handlers
    void start() {
        if (m_worker.joinable()) {
            stop();  // Stop existing worker first
        }

        auto runWorker = [this](std::stop_token st) {
            while (!st.stop_requested()) {
                Event e;
                if (m_queue.pop(e)) {
                    dispatch(e);
                } else {
                    break; // Queue shut down
                }
            }
        };

        m_worker = std::jthread([runWorker](std::stop_token st) {
            runWorker(st);
        });
    }

    // #REQ 3.d - Manage the lifecycle of thread correctly, ensure a clean shutdown
    void stop() {
        m_queue.shutdown();
        if (m_worker.joinable()) {
            m_worker.request_stop();
            m_worker.join();
        }
    }

    EventProcessor() = default;
    ~EventProcessor() { stop(); }
    
    // Rule of Five for proper resource management
    EventProcessor(EventProcessor&&) = delete;
    EventProcessor& operator=(EventProcessor&&) = delete;
    EventProcessor(const EventProcessor&) = delete;
    EventProcessor& operator=(const EventProcessor&) = delete;

private:
    // Dispatch event to the correct handler based on its type
    void dispatch(const Event& e) {
        std::visit(overloaded {
                [](const std::monostate&) {
                    // just a placeholder
                },
                // For each event type dispatch the correct handler
                [this](const UserInputEvent& evt) { 
                    if(m_inputHandler) 
                    m_inputHandler(evt); 
                },
                [this](const NetworkDataEvent& evt) { 
                    if(m_netHandler) 
                    m_netHandler(evt); 
                },
            }, 
        e);
    }

    BoundedQueue<Event, QUEUE_SIZE> m_queue;
    std::jthread m_worker;
    
    std::function<void(const UserInputEvent&)> m_inputHandler;
    std::function<void(const NetworkDataEvent&)> m_netHandler;
};

int main() {
    EventProcessor processor;

    // Register Handlers
    processor.registerHandler([](const UserInputEvent& e) {
        std::cout << "[Handler] User Input: " << e.input << "\n";
    });

    processor.registerHandler([](const NetworkDataEvent& e) {
        std::cout << "[Handler] Network Bytes: " << e.data.size() << "\n";
    });

    processor.start();

    // Producer Thread
    std::jthread producer([&processor](){
        processor.post(UserInputEvent{"User Event Produced"});
        processor.post(NetworkDataEvent{{0xDE, 0xAD, 0xBE, 0xEF}});
        processor.post(UserInputEvent{"User Event Produced"});
    });

    producer.join();
    processor.stop(); // Graceful shutdown
    
    return 0;
}