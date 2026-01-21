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
    std::cout << "\n=== Shallow Copy Example (Conceptual) ===\n";
    std::cout << "// Shallow copy with raw pointers causes double-free:\n";
    std::cout << "// class ShallowCopyResource {\n";
    std::cout << "//     int* data_;\n";
    std::cout << "//     // Default copy constructor does shallow copy:\n";
    std::cout << "//     // ShallowCopyResource(const ShallowCopyResource& other)\n";
    std::cout << "//     //     : data_(other.data_) {}  // BOTH point to same memory!\n";
    std::cout << "// };\n";
    std::cout << "// ShallowCopyResource a(42);\n";
    std::cout << "// ShallowCopyResource b = a;  // Shallow copy - same pointer!\n";
    std::cout << "// When a and b are destroyed, the SAME memory is freed twice - CRASH!\n\n";
    
    // Safe demonstration without actual double-free
    std::cout << "Safe demonstration of the concept:\n";
    int* shared_data = new int(42);
    int* ptr_a = shared_data;
    int* ptr_b = shared_data;  // Both point to same memory
    std::cout << "ptr_a points to: " << ptr_a << " (value: " << *ptr_a << ")\n";
    std::cout << "ptr_b points to: " << ptr_b << " (value: " << *ptr_b << ")\n";
    std::cout << "ptr_a == ptr_b: " << (ptr_a == ptr_b ? "true" : "false") << " (same address!)\n";
    *ptr_b = 100;  // Modifying via ptr_b
    std::cout << "After *ptr_b = 100: *ptr_a = " << *ptr_a << " (both see the change!)\n";
    std::cout << "DANGER: If both tried to delete, we'd have double-free!\n";
    delete shared_data;  // Only delete once to avoid crash
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
