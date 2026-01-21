#include "CopyAndSwapIdiomSample.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

// Helper class: Resource demonstrating copy and swap idiom
class CopyAndSwapIdiomSample::Resource {
private:
    int* data_;
    std::vector<int> values_;

public:
    explicit Resource(int value = 0) : data_(new int(value)) {
        values_.push_back(value);
        std::cout << "Resource constructed with value " << value << "\n";
    }

    // Copy constructor
    Resource(const Resource& other) : data_(new int(*other.data_)), values_(other.values_) {
        std::cout << "Resource copy-constructed\n";
    }

    // Move constructor (for completeness)
    Resource(Resource&& other) noexcept : data_(other.data_), values_(std::move(other.values_)) {
        other.data_ = nullptr;
        std::cout << "Resource move-constructed\n";
    }

    // Copy and swap assignment operator
    Resource& operator=(Resource other) noexcept {
        std::cout << "Resource copy-and-swap assignment\n";
        swap(*this, other);
        return *this;
    }

    // Swap function (friend for access to private members)
    friend void swap(Resource& first, Resource& second) noexcept {
        using std::swap;
        swap(first.data_, second.data_);
        swap(first.values_, second.values_);
        std::cout << "Resources swapped\n";
    }

    ~Resource() {
        std::cout << "Resource destroyed\n";
        delete data_;
    }

    void setValue(int value) {
        if (data_) *data_ = value;
        if (!values_.empty()) values_[0] = value;
    }

    int getValue() const { return data_ ? *data_ : 0; }

    void addValue(int value) {
        values_.push_back(value);
    }

    const std::vector<int>& getValues() const { return values_; }
};

void CopyAndSwapIdiomSample::demonstrate_copy_and_swap() {
    std::cout << "\n=== Copy and Swap Idiom ===\n";

    std::cout << "Creating resource a:\n";
    Resource a(42);
    a.addValue(1);
    a.addValue(2);

    std::cout << "\nCreating resource b:\n";
    Resource b(100);
    b.addValue(3);

    std::cout << "\nBefore assignment: a.value = " << a.getValue() << ", b.value = " << b.getValue() << "\n";

    std::cout << "\nAssigning b to a (copy and swap):\n";
    a = b;  // This uses copy and swap

    std::cout << "\nAfter assignment: a.value = " << a.getValue() << ", b.value = " << b.getValue() << "\n";
    std::cout << "a.values: ";
    for (int v : a.getValues()) std::cout << v << " ";
    std::cout << "\nb.values: ";
    for (int v : b.getValues()) std::cout << v << " ";
    std::cout << "\n";
}

void CopyAndSwapIdiomSample::demonstrate_exception_safety() {
    std::cout << "\n=== Exception Safety ===\n";
    std::cout << "Copy and swap provides strong exception safety:\n";
    std::cout << "- If copy construction fails, original object is unchanged\n";
    std::cout << "- If swap fails (unlikely), temporary is cleaned up\n";
    std::cout << "- Assignment either succeeds completely or fails completely\n";
}

void CopyAndSwapIdiomSample::demonstrate_best_practices() {
    std::cout << "\n=== Best Practices ===\n";
    std::cout << "- Use copy and swap for assignment operators\n";
    std::cout << "- Make swap noexcept for efficiency\n";
    std::cout << "- Implement both copy and move constructors\n";
    std::cout << "- Use ADL (Argument Dependent Lookup) for swap\n";
    std::cout << "- Consider using std::swap for standard types\n";
}

void CopyAndSwapIdiomSample::run() {
    std::cout << "Running Copy and Swap Idiom Sample...\n";
    demonstrate_copy_and_swap();
    demonstrate_exception_safety();
    demonstrate_best_practices();
    std::cout << "\nCopy and Swap Idiom demonstration completed!\n";
}
