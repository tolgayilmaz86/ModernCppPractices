#include "SmartPointersSample.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>

// ============================================================================
// Example Classes for Demonstration
// ============================================================================

class Resource {
private:
    std::string name_;
    static int instance_count_;

public:
    explicit Resource(const std::string& name) : name_(name) {
        instance_count_++;
        std::cout << "Resource '" << name_ << "' created. Total instances: " << instance_count_ << std::endl;
    }

    ~Resource() {
        instance_count_--;
        std::cout << "Resource '" << name_ << "' destroyed. Total instances: " << instance_count_ << std::endl;
    }

    void use() const {
        std::cout << "Using resource '" << name_ << "'" << std::endl;
    }

    const std::string& name() const { return name_; }

    static int getInstanceCount() { return instance_count_; }
};

int Resource::instance_count_ = 0;

// Custom deleter example
class FileHandler {
private:
    std::string filename_;
    bool opened_ = false;

public:
    explicit FileHandler(const std::string& filename) : filename_(filename) {
        // Simulate file opening
        opened_ = true;
        std::cout << "File '" << filename_ << "' opened" << std::endl;
    }

    ~FileHandler() {
        if (opened_) {
            close();
        }
    }

    void write(const std::string& data) {
        if (opened_) {
            std::cout << "Writing to file '" << filename_ << "': " << data << std::endl;
        }
    }

    void close() {
        if (opened_) {
            std::cout << "File '" << filename_ << "' closed" << std::endl;
            opened_ = false;
        }
    }
};

// ============================================================================
// Custom Deleter Examples
// ============================================================================

// Function pointer deleter
void customDeleter(Resource* ptr) {
    std::cout << "Custom deleter called for resource '" << ptr->name() << "'" << std::endl;
    delete ptr;
}

// Functor deleter
struct VerboseDeleter {
    void operator()(Resource* ptr) const {
        std::cout << "Verbose deleter: cleaning up '" << ptr->name() << "'" << std::endl;
        delete ptr;
    }
};

// ============================================================================
// Example 1: std::unique_ptr - Exclusive Ownership
// ============================================================================

void demonstrateUniquePtr() {
    std::cout << "\n=== std::unique_ptr - Exclusive Ownership ===" << std::endl;

    // Basic usage
    std::cout << "Creating unique_ptr..." << std::endl;
    std::unique_ptr<Resource> res1 = std::make_unique<Resource>("UniqueResource");

    res1->use();

    // Transfer ownership
    std::cout << "\nTransferring ownership..." << std::endl;
    std::unique_ptr<Resource> res2 = std::move(res1);

    if (!res1) {
        std::cout << "res1 is now null" << std::endl;
    }

    res2->use();

    // Custom deleter
    std::cout << "\nUsing custom deleter..." << std::endl;
    {
        std::unique_ptr<Resource, VerboseDeleter> res3(new Resource("CustomDelete"), VerboseDeleter{});
        res3->use();
    } // Resource automatically cleaned up with custom deleter

    std::cout << "unique_ptr demonstration completed" << std::endl;
}

// ============================================================================
// Example 2: std::shared_ptr - Shared Ownership
// ============================================================================

void demonstrateSharedPtr() {
    std::cout << "\n=== std::shared_ptr - Shared Ownership ===" << std::endl;

    // Basic usage
    std::cout << "Creating shared_ptr..." << std::endl;
    std::shared_ptr<Resource> res1 = std::make_shared<Resource>("SharedResource");

    std::cout << "Use count: " << res1.use_count() << std::endl;

    // Share ownership
    {
        std::shared_ptr<Resource> res2 = res1;
        std::cout << "After sharing - Use count: " << res1.use_count() << std::endl;

        res2->use();

        {
            std::shared_ptr<Resource> res3 = res2;
            std::cout << "After another share - Use count: " << res1.use_count() << std::endl;
            res3->use();
        }

        std::cout << "After res3 destroyed - Use count: " << res1.use_count() << std::endl;
    }

    std::cout << "After res2 destroyed - Use count: " << res1.use_count() << std::endl;

    // Custom deleter with shared_ptr
    std::cout << "\nUsing custom deleter with shared_ptr..." << std::endl;
    {
        std::shared_ptr<Resource> res4(new Resource("SharedCustom"), customDeleter);
        res4->use();
    }

    std::cout << "shared_ptr demonstration completed" << std::endl;
}

// ============================================================================
// Example 3: std::weak_ptr - Non-owning References
// ============================================================================

void demonstrateWeakPtr() {
    std::cout << "\n=== std::weak_ptr - Non-owning References ===" << std::endl;

    std::shared_ptr<Resource> shared = std::make_shared<Resource>("WeakDemo");
    std::cout << "Shared use count: " << shared.use_count() << std::endl;

    // Create weak pointer
    std::weak_ptr<Resource> weak = shared;
    std::cout << "After creating weak_ptr - Shared use count: " << shared.use_count() << std::endl;

    // Use weak pointer safely
    if (auto locked = weak.lock()) {
        std::cout << "Weak pointer locked successfully" << std::endl;
        locked->use();
    } else {
        std::cout << "Weak pointer expired" << std::endl;
    }

    // Demonstrate expiration
    std::cout << "\nResetting shared_ptr..." << std::endl;
    shared.reset();

    if (auto locked = weak.lock()) {
        std::cout << "Weak pointer still valid" << std::endl;
        locked->use();
    } else {
        std::cout << "Weak pointer expired after shared_ptr reset" << std::endl;
    }

    std::cout << "weak_ptr demonstration completed" << std::endl;
}

// ============================================================================
// Example 4: Circular References Problem and Solution
// ============================================================================

class Parent;
class Child;

class Parent {
public:
    std::shared_ptr<Child> child;
    ~Parent() { std::cout << "Parent destroyed" << std::endl; }
};

class Child {
public:
    std::shared_ptr<Parent> parent;
    ~Child() { std::cout << "Child destroyed" << std::endl; }
};

class WeakParent;
class WeakChild;

class WeakParent {
public:
    std::shared_ptr<WeakChild> child;
    std::weak_ptr<WeakParent> self;  // For demonstration
    ~WeakParent() { std::cout << "WeakParent destroyed" << std::endl; }
};

class WeakChild {
public:
    std::weak_ptr<WeakParent> parent;  // Use weak_ptr to break cycle
    ~WeakChild() { std::cout << "WeakChild destroyed" << std::endl; }
};

void demonstrateCircularReferences() {
    std::cout << "\n=== Circular References Problem ===" << std::endl;

    std::cout << "Creating circular reference with shared_ptr..." << std::endl;
    {
        auto parent = std::make_shared<Parent>();
        auto child = std::make_shared<Child>();

        parent->child = child;
        child->parent = parent;

        std::cout << "Parent use count: " << parent.use_count() << std::endl;
        std::cout << "Child use count: " << child.use_count() << std::endl;
    }
    std::cout << "Objects should be destroyed now, but circular reference prevents it!" << std::endl;
    std::cout << "Total Resource instances: " << Resource::getInstanceCount() << std::endl;

    std::cout << "\nSolution: Using weak_ptr to break cycles..." << std::endl;
    {
        auto parent = std::make_shared<WeakParent>();
        auto child = std::make_shared<WeakChild>();

        parent->child = child;
        child->parent = parent;  // weak_ptr doesn't increase reference count

        std::cout << "WeakParent use count: " << parent.use_count() << std::endl;
        std::cout << "WeakChild use count: " << child.use_count() << std::endl;
    }
    std::cout << "Objects properly destroyed with weak_ptr solution" << std::endl;
}

// ============================================================================
// Example 5: Array Support
// ============================================================================

void demonstrateArrays() {
    std::cout << "\n=== Array Support ===" << std::endl;

    // unique_ptr for arrays
    std::cout << "unique_ptr for arrays..." << std::endl;
    {
        std::unique_ptr<Resource[]> resources(new Resource[3]{Resource("Array1"), Resource("Array2"), Resource("Array3")});
        for (int i = 0; i < 3; ++i) {
            resources[i].use();
        }
    }

    // shared_ptr for arrays (C++17)
    std::cout << "\nshared_ptr for arrays..." << std::endl;
    {
        std::shared_ptr<Resource[]> shared_resources(new Resource[2]{Resource("SharedArray1"), Resource("SharedArray2")},
                                                     std::default_delete<Resource[]>());
        for (int i = 0; i < 2; ++i) {
            shared_resources[i].use();
        }
    }

    std::cout << "Array demonstration completed" << std::endl;
}

// ============================================================================
// Example 6: Best Practices and Common Pitfalls
// ============================================================================

void demonstrateBestPractices() {
    std::cout << "\n=== Best Practices and Common Pitfalls ===" << std::endl;

    // ✅ Good: Use make_unique and make_shared
    auto good_unique = std::make_unique<Resource>("GoodUnique");
    auto good_shared = std::make_shared<Resource>("GoodShared");

    // ❌ Bad: Don't use new with smart pointers directly
    // auto bad_unique = std::unique_ptr<Resource>(new Resource("Bad"));

    // ✅ Good: Use weak_ptr to observe shared objects
    std::weak_ptr<Resource> observer = good_shared;
    if (auto locked = observer.lock()) {
        locked->use();
    }

    // ✅ Good: Be careful with raw pointers in smart pointer contexts
    std::cout << "\nRaw pointer considerations:" << std::endl;
    Resource* raw_ptr = good_unique.get();  // Get raw pointer
    raw_ptr->use();  // Use it carefully - don't delete!

    // ✅ Good: Use custom deleters when needed
    auto file_ptr = std::unique_ptr<FileHandler>(new FileHandler("example.txt"));
    file_ptr->write("Hello, smart pointers!");

    std::cout << "Best practices demonstration completed" << std::endl;
}

// ============================================================================
// Example 7: Performance Considerations
// ============================================================================

void demonstratePerformance() {
    std::cout << "\n=== Performance Considerations ===" << std::endl;

    std::cout << "Memory overhead:" << std::endl;
    std::cout << "- unique_ptr: Minimal (usually same as raw pointer)" << std::endl;
    std::cout << "- shared_ptr: Reference counter (typically 16-32 bytes)" << std::endl;
    std::cout << "- weak_ptr: Additional weak count in control block" << std::endl;

    std::cout << "\nPerformance characteristics:" << std::endl;
    std::cout << "- make_shared: Allocates object and control block together" << std::endl;
    std::cout << "- shared_ptr from unique_ptr: Creates new control block" << std::endl;
    std::cout << "- weak_ptr.lock(): Atomic operations for thread safety" << std::endl;

    // Demonstrate make_shared vs new + shared_ptr
    std::cout << "\nmake_shared vs new + shared_ptr:" << std::endl;
    {
        // Better: single allocation
        auto efficient = std::make_shared<Resource>("Efficient");
    }
    {
        // Worse: two allocations (object + control block)
        auto inefficient = std::shared_ptr<Resource>(new Resource("Inefficient"));
    }

    std::cout << "Performance demonstration completed" << std::endl;
}

void SmartPointersSample::run() {
    std::cout << "Running Smart Pointers Sample..." << std::endl;

    demonstrateUniquePtr();
    demonstrateSharedPtr();
    demonstrateWeakPtr();
    demonstrateCircularReferences();
    demonstrateArrays();
    demonstrateBestPractices();
    demonstratePerformance();

    std::cout << "\n=== Smart Pointers Summary ===" << std::endl;
    std::cout << "std::unique_ptr:" << std::endl;
    std::cout << "- Exclusive ownership" << std::endl;
    std::cout << "- Zero overhead" << std::endl;
    std::cout << "- Cannot be copied" << std::endl;
    std::cout << "- Use for sole ownership" << std::endl;

    std::cout << "\nstd::shared_ptr:" << std::endl;
    std::cout << "- Shared ownership" << std::endl;
    std::cout << "- Reference counting" << std::endl;
    std::cout << "- Thread-safe reference counting" << std::endl;
    std::cout << "- Use when multiple owners needed" << std::endl;

    std::cout << "\nstd::weak_ptr:" << std::endl;
    std::cout << "- Non-owning reference" << std::endl;
    std::cout << "- Breaks circular references" << std::endl;
    std::cout << "- Must be locked to access" << std::endl;
    std::cout << "- Use for observation without ownership" << std::endl;

    std::cout << "\nWhen to use raw pointers:" << std::endl;
    std::cout << "- Non-owning observations" << std::endl;
    std::cout << "- Performance-critical code" << std::endl;
    std::cout << "- Interfacing with C APIs" << std::endl;
    std::cout << "- Inside smart pointer implementations" << std::endl;

    std::cout << "\nSmart pointers best practices:" << std::endl;
    std::cout << "- Prefer make_unique and make_shared" << std::endl;
    std::cout << "- Use unique_ptr by default" << std::endl;
    std::cout << "- Avoid circular references" << std::endl;
    std::cout << "- Be careful with raw pointers from get()" << std::endl;
    std::cout << "- Use custom deleters when necessary" << std::endl;

    std::cout << "\nSmart pointers demonstration completed!" << std::endl;
}