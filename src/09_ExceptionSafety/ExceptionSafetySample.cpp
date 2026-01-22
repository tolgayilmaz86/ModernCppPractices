#include "ExceptionSafetySample.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <expected>

// ============================================================================
// Example Classes for Demonstration
// ============================================================================

class SafetyResource {
private:
    std::string name_;
    static int instance_count_;

public:
    explicit SafetyResource(const std::string& name) : name_(name) {
        instance_count_++;
        std::cout << "SafetyResource '" << name_ << "' acquired. Total instances: " << instance_count_ << std::endl;
    }

    ~SafetyResource() {
        instance_count_--;
        std::cout << "SafetyResource '" << name_ << "' released. Total instances: " << instance_count_ << std::endl;
    }

    void use() const {
        std::cout << "Using safety resource '" << name_ << "'" << std::endl;
    }

    const std::string& name() const { return name_; }

    static int getInstanceCount() { return instance_count_; }

    // Simulate an operation that might throw
    void risky_operation() {
        if (name_ == "RiskySafetyResource") {
            throw std::runtime_error("Risky operation failed!");
        }
        std::cout << "Risky operation succeeded on '" << name_ << "'" << std::endl;
    }
};

int SafetyResource::instance_count_ = 0;

// ============================================================================
// Exception Safety Levels Demonstration
// ============================================================================

class ExceptionSafetyLevels {
private:
    std::vector<std::shared_ptr<SafetyResource>> resources_;  // Use shared_ptr for copyability

public:
    // No exception safety guarantee - if push_back throws, resources_ is corrupted
    void add_resource_no_guarantee(const std::string& name) {
        resources_.push_back(std::make_shared<SafetyResource>(name));
        // If this throws, the resource is leaked and vector might be corrupted
        throw std::runtime_error("Simulated failure after adding resource");
    }

    // Basic exception safety - invariants are preserved, but state may change
    void add_resource_basic_guarantee(const std::string& name) {
        auto resource = std::make_shared<SafetyResource>(name);
        resources_.push_back(resource);  // Strong guarantee for push_back
        // If this throws, resource is properly cleaned up, but it's already in the vector
        throw std::runtime_error("Simulated failure after adding resource");
    }

    // Strong exception safety - operation either succeeds completely or fails completely
    void add_resource_strong_guarantee(const std::string& name) {
        auto temp_resources = resources_;  // Copy current state (now possible with shared_ptr)
        temp_resources.push_back(std::make_shared<SafetyResource>(name));

        // Simulate some work that might fail
        if (name == "FailStrong") {
            throw std::runtime_error("Strong guarantee: operation failed, rolling back");
        }

        // Only commit if everything succeeds
        resources_ = std::move(temp_resources);
        std::cout << "Strong guarantee: operation completed successfully" << std::endl;
    }

    // No-throw guarantee - operation never throws
    void add_resource_no_throw(const std::string& name) noexcept {
        try {
            resources_.push_back(std::make_shared<SafetyResource>(name));
            std::cout << "No-throw guarantee: resource added successfully" << std::endl;
        } catch (...) {
            // In real code, you might log the error but never let it escape
            std::cout << "No-throw guarantee: swallowed exception (this is usually bad!)" << std::endl;
        }
    }

    void list_resources() const {
        std::cout << "Current resources (" << resources_.size() << "):" << std::endl;
        for (const auto& res : resources_) {
            std::cout << "  - " << res->name() << std::endl;
        }
    }

    size_t size() const { return resources_.size(); }
};

// ============================================================================
// RAII for Exception Safety
// ============================================================================

class SafeFile {
private:
    std::string filename_;
    bool opened_ = false;

public:
    explicit SafeFile(const std::string& filename) : filename_(filename) {
        // Simulate file opening
        opened_ = true;
        std::cout << "File '" << filename_ << "' opened" << std::endl;
    }

    ~SafeFile() {
        close();
    }

    void write(const std::string& data) {
        if (!opened_) throw std::runtime_error("File not opened");
        std::cout << "Writing to file '" << filename_ << "': " << data << std::endl;
    }

    void close() {
        if (opened_) {
            std::cout << "File '" << filename_ << "' closed" << std::endl;
            opened_ = false;
        }
    }
};

class Transaction {
private:
    std::vector<std::function<void()>> rollbacks_;
    bool committed_ = false;

public:
    ~Transaction() {
        if (!committed_) {
            std::cout << "Transaction failed - rolling back..." << std::endl;
            // Rollback in reverse order
            for (auto it = rollbacks_.rbegin(); it != rollbacks_.rend(); ++it) {
                try {
                    (*it)();
                } catch (...) {
                    std::cout << "Rollback action failed - continuing..." << std::endl;
                }
            }
        }
    }

    void add_rollback(std::function<void()> rollback) {
        rollbacks_.push_back(rollback);
    }

    void commit() {
        committed_ = true;
        std::cout << "Transaction committed successfully" << std::endl;
    }
};

// ============================================================================
// Exception-Safe Container Operations
// ============================================================================

class SafeVector {
private:
    std::vector<int> data_;

public:
    // Exception-safe push_back with strong guarantee
    void push_back_safe(int value) {
        // std::vector push_back provides strong guarantee
        data_.push_back(value);
        std::cout << "Safely added " << value << " to vector" << std::endl;
    }

    // Exception-safe insert with strong guarantee
    void insert_safe(size_t pos, int value) {
        if (pos > data_.size()) {
            throw std::out_of_range("Invalid position");
        }
        data_.insert(data_.begin() + pos, value);
        std::cout << "Safely inserted " << value << " at position " << pos << std::endl;
    }

    // Exception-safe resize with strong guarantee
    void resize_safe(size_t new_size, int default_value = 0) {
        data_.resize(new_size, default_value);
        std::cout << "Safely resized vector to " << new_size << " elements" << std::endl;
    }

    void print() const {
        std::cout << "Vector contents: [";
        for (size_t i = 0; i < data_.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data_[i];
        }
        std::cout << "]" << std::endl;
    }
};

// ============================================================================
// Exception Specifications and noexcept
// ============================================================================

class ExceptionSpecifications {
public:
    // Noexcept function - promises not to throw
    void no_throw_operation() noexcept {
        std::cout << "This operation never throws" << std::endl;
    }

    // Conditionally noexcept (using template)
    template <int N>
    void conditional_noexcept() noexcept(N >= 0) {
        if constexpr (N < 0) {
            throw std::invalid_argument("Negative value not allowed");
        } else {
            std::cout << "Processed non-negative value: " << N << std::endl;
        }
    }

    // Function that may throw
    void may_throw_operation(int value) {
        if (value == 42) {
            throw std::runtime_error("The answer is not allowed!");
        }
        std::cout << "Processed value: " << value << std::endl;
    }
};

// ============================================================================
// Exception Safety Testing
// ============================================================================

class ExceptionSafetyTester {
public:
    static void test_basic_guarantee() {
        std::cout << "\n=== Testing Basic Exception Safety Guarantee ===" << std::endl;

        ExceptionSafetyLevels levels;
        try {
            levels.add_resource_basic_guarantee("BasicTest");
        } catch (const std::exception& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
            std::cout << "Basic guarantee: Object state is valid but may have changed" << std::endl;
        }
        levels.list_resources();
    }

    static void test_strong_guarantee() {
        std::cout << "\n=== Testing Strong Exception Safety Guarantee ===" << std::endl;

        ExceptionSafetyLevels levels;
        try {
            levels.add_resource_strong_guarantee("StrongTest");
            levels.add_resource_strong_guarantee("FailStrong");  // This will fail
        } catch (const std::exception& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
            std::cout << "Strong guarantee: Operation either succeeded completely or failed completely" << std::endl;
        }
        levels.list_resources();
    }

    static void test_no_throw_guarantee() {
        std::cout << "\n=== Testing No-Throw Guarantee ===" << std::endl;

        ExceptionSafetyLevels levels;
        levels.add_resource_no_throw("NoThrowTest");
        std::cout << "No-throw guarantee: Function completed without throwing" << std::endl;
        levels.list_resources();
    }
};

// ============================================================================
// Demonstration Functions
// ============================================================================

void demonstrate_exception_safety_levels() {
    std::cout << "\n=== Exception Safety Guarantees ===" << std::endl;

    ExceptionSafetyTester::test_basic_guarantee();
    ExceptionSafetyTester::test_strong_guarantee();
    ExceptionSafetyTester::test_no_throw_guarantee();
}

void demonstrate_raii_exception_safety() {
    std::cout << "\n=== RAII for Exception Safety ===" << std::endl;

    try {
        SafeFile file("example.txt");
        file.write("Hello, World!");

        // Simulate an exception
        throw std::runtime_error("Something went wrong!");

        // File will be automatically closed by destructor
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        std::cout << "File was automatically closed by RAII" << std::endl;
    }
}

void demonstrate_transaction_pattern() {
    std::cout << "\n=== Transaction Pattern for Exception Safety ===" << std::endl;

    try {
        Transaction transaction;

        // Add some resources with rollback actions
        auto res1 = std::make_unique<SafetyResource>("TransactionResource1");
        transaction.add_rollback([]() {
            std::cout << "Rolling back: cleaning up TransactionResource1" << std::endl;
        });

        auto res2 = std::make_unique<SafetyResource>("TransactionResource2");
        transaction.add_rollback([]() {
            std::cout << "Rolling back: cleaning up TransactionResource2" << std::endl;
        });

        // Simulate failure
        throw std::runtime_error("Transaction failed!");

        // If we reach here, commit
        transaction.commit();

    } catch (const std::exception& e) {
        std::cout << "Transaction failed: " << e.what() << std::endl;
        std::cout << "All resources were automatically cleaned up" << std::endl;
    }
}

void demonstrate_container_exception_safety() {
    std::cout << "\n=== Container Exception Safety ===" << std::endl;

    SafeVector vec;

    try {
        vec.push_back_safe(1);
        vec.push_back_safe(2);
        vec.insert_safe(1, 99);
        vec.resize_safe(5, 42);

        vec.print();

        // Try an operation that might fail
        vec.insert_safe(10, 100);  // This will throw

    } catch (const std::exception& e) {
        std::cout << "Container operation failed: " << e.what() << std::endl;
        std::cout << "Container state remains valid:" << std::endl;
        vec.print();
    }
}

void demonstrate_noexcept_specifications() {
    std::cout << "\n=== Exception Specifications ===" << std::endl;

    ExceptionSpecifications specs;

    // Noexcept function
    specs.no_throw_operation();

    // Conditionally noexcept
    try {
        specs.conditional_noexcept<5>();   // Should succeed
        specs.conditional_noexcept<-1>();  // Will throw
    } catch (const std::exception& e) {
        std::cout << "Conditional noexcept threw: " << e.what() << std::endl;
    }

    // Function that may throw
    try {
        specs.may_throw_operation(10);  // Should succeed
        specs.may_throw_operation(42);  // Will throw
    } catch (const std::exception& e) {
        std::cout << "May-throw function threw: " << e.what() << std::endl;
    }
}

void demonstrate_exception_handling_best_practices() {
    std::cout << "\n=== Exception Handling Best Practices ===" << std::endl;

    // 1. Catch by const reference
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::exception& e) {
        std::cout << "Caught by const reference: " << e.what() << std::endl;
    }

    // 2. Catch most specific exceptions first
    try {
        throw std::invalid_argument("Invalid argument");
    } catch (const std::invalid_argument& e) {
        std::cout << "Caught specific exception: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught base exception: " << e.what() << std::endl;
    }

    // 3. Use RAII for resource management
    {
        auto resource = std::make_unique<SafetyResource>("RAII_Resource");
        resource->use();
        // Resource automatically cleaned up
    }

    // 4. Don't throw in destructors (unless you catch)
    class SafeDestructor {
    public:
        ~SafeDestructor() noexcept {
            try {
                // Code that might throw
                std::cout << "Safe destructor cleanup" << std::endl;
            } catch (...) {
                // Never let exceptions escape destructor
                std::cout << "Destructor swallowed exception" << std::endl;
            }
        }
    };

    SafeDestructor sd;
}

// ============================================================================
// std::expected Demonstration (C++23)
// ============================================================================

// Error types for file operations
enum class FileReadError {
    FailedToOpen,
    MissingPermissions,
    FileCorrupted
};

// Error types for processing operations
enum class ProcessingError {
    InvalidData,
    Overflow,
    DivisionByZero
};

// Read a number from file or return an error on failure
std::expected<double, FileReadError> readDoubleFromFile(std::string_view path) {
    // Simulate different error conditions
    if (path == "missing.txt") {
        return std::unexpected(FileReadError::FailedToOpen);
    } else if (path == "noperm.txt") {
        return std::unexpected(FileReadError::MissingPermissions);
    } else if (path == "corrupt.txt") {
        return std::unexpected(FileReadError::FileCorrupted);
    }

    // Simulate successful read
    return 42.5;
}

// Process the read value (validate and transform)
std::expected<double, ProcessingError> validateAndProcess(double value) {
    if (value < 0) {
        return std::unexpected(ProcessingError::InvalidData);
    } else if (value > 1000) {
        return std::unexpected(ProcessingError::Overflow);
    }

    // Apply some processing
    return value * 2.0;
}

// Divide by another value
std::expected<double, ProcessingError> divideBy(double numerator, double denominator) {
    if (denominator == 0) {
        return std::unexpected(ProcessingError::DivisionByZero);
    }
    return numerator / denominator;
}

// Convert errors to strings for display
std::string errorToString(FileReadError err) {
    switch (err) {
        case FileReadError::FailedToOpen: return "Failed to open file";
        case FileReadError::MissingPermissions: return "Missing permissions";
        case FileReadError::FileCorrupted: return "File is corrupted";
    }
    return "Unknown file error";
}

std::string errorToString(ProcessingError err) {
    switch (err) {
        case ProcessingError::InvalidData: return "Invalid data";
        case ProcessingError::Overflow: return "Value overflow";
        case ProcessingError::DivisionByZero: return "Division by zero";
    }
    return "Unknown processing error";
}

// Traditional approach - nested if/else with early returns
double traditionalApproach(std::string_view filename, double divisor) {
    // Read from file
    auto readResult = readDoubleFromFile(filename);
    if (!readResult.has_value()) {
        throw std::runtime_error("File read failed: " + errorToString(readResult.error()));
    }

    double value = readResult.value();

    // Validate and process
    auto processedResult = validateAndProcess(value);
    if (!processedResult.has_value()) {
        throw std::runtime_error("Processing failed: " + errorToString(processedResult.error()));
    }

    double processed = processedResult.value();

    // Divide
    auto finalResult = divideBy(processed, divisor);
    if (!finalResult.has_value()) {
        throw std::runtime_error("Division failed: " + errorToString(finalResult.error()));
    }

    return finalResult.value();
}

// Monadic approach using std::expected
std::expected<double, std::string> monadicApproach(std::string_view filename, double divisor) {
    return readDoubleFromFile(filename)
        // Transform FileReadError to string immediately
        .or_else([](FileReadError err) -> std::expected<double, std::string> {
            return std::unexpected(errorToString(err));
        })
        // On success, validate and process the value
        .and_then([](double val) -> std::expected<double, std::string> {
            return validateAndProcess(val)
                .or_else([](ProcessingError err) -> std::expected<double, std::string> {
                    return std::unexpected(errorToString(err));
                });
        })
        // On success, divide by divisor
        .and_then([divisor](double val) -> std::expected<double, std::string> {
            return divideBy(val, divisor)
                .or_else([](ProcessingError err) -> std::expected<double, std::string> {
                    return std::unexpected(errorToString(err));
                });
        });
}

// Alternative: transform for value transformation
std::expected<int, std::string> transformExample(double input) {
    return readDoubleFromFile("test.txt")
        .or_else([](FileReadError err) -> std::expected<double, std::string> {
            return std::unexpected(errorToString(err));
        })
        .and_then([](double val) -> std::expected<double, std::string> {
            return validateAndProcess(val)
                .or_else([](ProcessingError err) -> std::expected<double, std::string> {
                    return std::unexpected(errorToString(err));
                });
        })
        .transform([](double val) -> int {
            return static_cast<int>(val);
        });
}

void demonstrate_std_expected() {
    std::cout << "\n=== std::expected - Monadic Error Handling (C++23) ===" << std::endl;

    // Demonstrate traditional approach (throws exceptions)
    std::cout << "\n--- Traditional Approach (with exceptions) ---" << std::endl;
    try {
        double result = traditionalApproach("valid.txt", 2.0);
        std::cout << "Traditional result: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Traditional approach failed: " << e.what() << std::endl;
    }

    try {
        double result = traditionalApproach("missing.txt", 2.0);
        std::cout << "Traditional result: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Traditional approach failed: " << e.what() << std::endl;
    }

    // Demonstrate monadic approach (composable, no exceptions)
    std::cout << "\n--- Monadic Approach (with std::expected) ---" << std::endl;

    // Successful case
    auto successResult = monadicApproach("valid.txt", 2.0);
    if (successResult.has_value()) {
        std::cout << "Monadic success: " << successResult.value() << std::endl;
    } else {
        std::cout << "Monadic failed: " << successResult.error() << std::endl;
    }

    // File read failure
    auto fileErrorResult = monadicApproach("missing.txt", 2.0);
    if (fileErrorResult.has_value()) {
        std::cout << "Monadic success: " << fileErrorResult.value() << std::endl;
    } else {
        std::cout << "Monadic failed: " << fileErrorResult.error() << std::endl;
    }

    // Processing failure
    auto processErrorResult = monadicApproach("corrupt.txt", 2.0);
    if (processErrorResult.has_value()) {
        std::cout << "Monadic success: " << processErrorResult.value() << std::endl;
    } else {
        std::cout << "Monadic failed: " << processErrorResult.error() << std::endl;
    }

    // Division by zero
    auto divisionErrorResult = monadicApproach("valid.txt", 0.0);
    if (divisionErrorResult.has_value()) {
        std::cout << "Monadic success: " << divisionErrorResult.value() << std::endl;
    } else {
        std::cout << "Monadic failed: " << divisionErrorResult.error() << std::endl;
    }

    // Demonstrate transform
    std::cout << "\n--- Transform Operation ---" << std::endl;
    auto transformResult = transformExample(42.0);
    if (transformResult.has_value()) {
        std::cout << "Transform result: " << transformResult.value() << std::endl;
    } else {
        std::cout << "Transform failed: " << transformResult.error() << std::endl;
    }

    std::cout << "\nstd::expected advantages:" << std::endl;
    std::cout << "- Composable operations with and_then()" << std::endl;
    std::cout << "- Error handling with or_else()" << std::endl;
    std::cout << "- Value transformation with transform()" << std::endl;
    std::cout << "- No exception overhead in success path" << std::endl;
    std::cout << "- Type-safe error handling" << std::endl;
    std::cout << "- Clear intent: success/failure is explicit" << std::endl;
}

#include "SampleRegistry.hpp"

void ExceptionSafetySample::run() {
    std::cout << "Running Exception Safety Sample..." << std::endl;

    demonstrate_exception_safety_levels();
    demonstrate_raii_exception_safety();
    demonstrate_transaction_pattern();
    demonstrate_container_exception_safety();
    demonstrate_noexcept_specifications();
    demonstrate_exception_handling_best_practices();
    demonstrate_std_expected();

    std::cout << "\n=== Exception Safety Summary ===" << std::endl;
    std::cout << "Exception Safety Guarantees:" << std::endl;
    std::cout << "- No guarantee: Operation may leave object in invalid state" << std::endl;
    std::cout << "- Basic guarantee: Invariants preserved, but state may change" << std::endl;
    std::cout << "- Strong guarantee: Operation succeeds completely or fails completely" << std::endl;
    std::cout << "- No-throw guarantee: Operation never throws exceptions" << std::endl;

    std::cout << "\nKey Principles:" << std::endl;
    std::cout << "- Use RAII for automatic resource cleanup" << std::endl;
    std::cout << "- Prefer strong exception safety when possible" << std::endl;
    std::cout << "- Use noexcept for functions that never throw" << std::endl;
    std::cout << "- Implement transaction-like operations for multi-step changes" << std::endl;
    std::cout << "- Test exception safety of your code" << std::endl;

    std::cout << "\nException Handling Best Practices:" << std::endl;
    std::cout << "- Catch by const reference" << std::endl;
    std::cout << "- Catch most specific exceptions first" << std::endl;
    std::cout << "- Never let exceptions escape destructors" << std::endl;
    std::cout << "- Use smart pointers and RAII containers" << std::endl;
    std::cout << "- Document exception specifications" << std::endl;

    std::cout << "\nModern C++ Exception Safety Features:" << std::endl;
    std::cout << "- std::unique_ptr and std::shared_ptr for automatic cleanup" << std::endl;
    std::cout << "- Container operations with strong guarantees" << std::endl;
    std::cout << "- noexcept specifications for optimization" << std::endl;
    std::cout << "- std::optional for operations that might fail" << std::endl;
    std::cout << "- std::expected for monadic error handling (C++23)" << std::endl;
    std::cout << "- RAII everywhere for exception safety" << std::endl;

    std::cout << "\nException safety demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(ExceptionSafetySample, "Exception Safety", 9);