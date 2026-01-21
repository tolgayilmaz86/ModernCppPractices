#include "DeepShallowCopySample.hpp"
#include <iostream>
#include <cstring>

// Helper class: Shallow copy (default copy)
class DeepShallowCopySample::ShallowCopyResource {
public:
    int* data_;
    explicit ShallowCopyResource(int value) : data_(new int(value)) {
        std::cout << "ShallowCopyResource constructed, value=" << *data_ << "\n";
    }
    ~ShallowCopyResource() {
        std::cout << "ShallowCopyResource destroyed, value=" << (data_ ? std::to_string(*data_) : "null") << "\n";
        delete data_;
    }
    // Default copy constructor/assignment (shallow)
};

// Helper class: Deep copy (custom copy)
class DeepShallowCopySample::DeepCopyResource {
public:
    int* data_;
    explicit DeepCopyResource(int value) : data_(new int(value)) {
        std::cout << "DeepCopyResource constructed, value=" << *data_ << "\n";
    }
    // Deep copy constructor
    DeepCopyResource(const DeepCopyResource& other) : data_(new int(*other.data_)) {
        std::cout << "DeepCopyResource deep-copied, value=" << *data_ << "\n";
    }
    // Deep copy assignment
    DeepCopyResource& operator=(const DeepCopyResource& other) {
        if (this != &other) {
            *data_ = *other.data_;
            std::cout << "DeepCopyResource deep-assigned, value=" << *data_ << "\n";
        }
        return *this;
    }
    ~DeepCopyResource() {
        std::cout << "DeepCopyResource destroyed, value=" << (data_ ? std::to_string(*data_) : "null") << "\n";
        delete data_;
    }
};

void DeepShallowCopySample::demonstrate_shallow_copy() {
    std::cout << "\n=== Shallow Copy Example ===\n";
    ShallowCopyResource a(42);
    ShallowCopyResource b = a; // shallow copy
    std::cout << "a.data_ = " << a.data_ << ", b.data_ = " << b.data_ << "\n";
    *b.data_ = 100;
    std::cout << "After modifying b, a.data_ = " << *a.data_ << ", b.data_ = " << *b.data_ << "\n";
}

void DeepShallowCopySample::demonstrate_deep_copy() {
    std::cout << "\n=== Deep Copy Example ===\n";
    DeepCopyResource a(42);
    DeepCopyResource b = a; // deep copy
    std::cout << "a.data_ = " << a.data_ << ", b.data_ = " << b.data_ << "\n";
    *b.data_ = 100;
    std::cout << "After modifying b, a.data_ = " << *a.data_ << ", b.data_ = " << *b.data_ << "\n";
}

void DeepShallowCopySample::demonstrate_rule_of_three_five() {
    std::cout << "\n=== Rule of Three/Five ===\n";
    std::cout << "If your class manages resources (raw pointers, file handles, etc.),\n";
    std::cout << "implement the Rule of Three (copy ctor, copy assignment, dtor) or\n";
    std::cout << "Rule of Five (add move ctor, move assignment).\n" << std::endl;
}

void DeepShallowCopySample::demonstrate_best_practices() {
    std::cout << "\n=== Best Practices ===\n";
    std::cout << "- Prefer smart pointers (std::unique_ptr, std::shared_ptr) for resource management.\n";
    std::cout << "- Avoid raw pointers unless necessary.\n";
    std::cout << "- Always implement deep copy for classes owning resources.\n";
    std::cout << "- Use =delete to prevent copying if not supported.\n";
}

void DeepShallowCopySample::run() {
    std::cout << "Running Deep vs Shallow Copy Sample...\n";
    demonstrate_shallow_copy();
    demonstrate_deep_copy();
    demonstrate_rule_of_three_five();
    demonstrate_best_practices();
    std::cout << "\nDeep vs Shallow Copy demonstration completed!\n";
}
