#include "TypeErasureSample.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>

// ============================================================================
// Example 1: Basic Type Erasure - Shape Drawing
// ============================================================================

// Abstract interface for drawable objects
class Drawable {
public:
    virtual ~Drawable() = default;
    virtual void draw() const = 0;
    virtual std::string name() const = 0;
};

// Concrete drawable shapes
class Circle : public Drawable {
private:
    double radius_;
public:
    explicit Circle(double r) : radius_(r) {}
    void draw() const override {
        std::cout << "Drawing circle with radius " << radius_ << std::endl;
    }
    std::string name() const override { return "Circle"; }
};

class Square : public Drawable {
private:
    double side_;
public:
    explicit Square(double s) : side_(s) {}
    void draw() const override {
        std::cout << "Drawing square with side " << side_ << std::endl;
    }
    std::string name() const override { return "Square"; }
};

class Triangle : public Drawable {
private:
    double base_, height_;
public:
    Triangle(double b, double h) : base_(b), height_(h) {}
    void draw() const override {
        std::cout << "Drawing triangle with base " << base_ << " and height " << height_ << std::endl;
    }
    std::string name() const override { return "Triangle"; }
};

// Simple test wrapper
class SimpleWrapper {
private:
    std::unique_ptr<Circle> circle_;
public:
    SimpleWrapper(double radius) : circle_(std::make_unique<Circle>(radius)) {}
    void draw() const { circle_->draw(); }
};

// ============================================================================
// Working Type Erasure Example - Simplified
// ============================================================================

// Base concept for any callable
class CallableBase {
public:
    virtual ~CallableBase() = default;
    virtual void call() = 0;
    virtual std::unique_ptr<CallableBase> clone() const = 0;
};

// Template model for any callable type
template <typename T>
class CallableModel : public CallableBase {
private:
    T callable_;
public:
    CallableModel(T c) : callable_(c) {}
    
    void call() override {
        callable_();
    }
    
    std::unique_ptr<CallableBase> clone() const override {
        return std::make_unique<CallableModel<T>>(callable_);
    }
};

// Type-erased callable wrapper (like std::function<void()>)
class AnyCallable {
private:
    std::unique_ptr<CallableBase> pimpl_;
public:
    template <typename T>
    AnyCallable(T callable) : pimpl_(std::make_unique<CallableModel<T>>(callable)) {}
    
    AnyCallable(const AnyCallable& other) : pimpl_(other.pimpl_->clone()) {}
    AnyCallable& operator=(const AnyCallable& other) {
        pimpl_ = other.pimpl_->clone();
        return *this;
    }
    
    void operator()() {
        pimpl_->call();
    }
};

// ============================================================================
// Example 2: Function Object Type Erasure (like std::function)
// ============================================================================

// Type-erased function wrapper
class FunctionObject {
private:
    // Concept for callable objects
    struct CallableConcept {
        virtual ~CallableConcept() = default;
        virtual void call() const = 0;
        virtual std::unique_ptr<CallableConcept> clone() const = 0;
    };

    // Model for any callable type
    template <typename T>
    struct CallableModel : CallableConcept {
        T callable_;
        explicit CallableModel(T c) : callable_(std::move(c)) {}

        void call() const override { callable_(); }

        std::unique_ptr<CallableConcept> clone() const override {
            return std::make_unique<CallableModel<T>>(callable_);
        }
    };

    std::unique_ptr<CallableConcept> pimpl_;

public:
    // Constructor accepting any callable
    template <typename T>
    FunctionObject(T callable) : pimpl_(std::make_unique<CallableModel<T>>(std::move(callable))) {}

    // Copy operations
    FunctionObject(const FunctionObject& other) : pimpl_(other.pimpl_->clone()) {}
    FunctionObject& operator=(const FunctionObject& other) {
        pimpl_ = other.pimpl_->clone();
        return *this;
    }

    // Interface
    void operator()() const { pimpl_->call(); }
};

// ============================================================================
// Example 3: Advanced Type Erasure with Multiple Operations
// ============================================================================

// Interface for objects that can be processed
class Processable {
public:
    virtual ~Processable() = default;
    virtual void process() = 0;
    virtual int getValue() const = 0;
    virtual void setValue(int val) = 0;
    virtual std::string getType() const = 0;
};

// Concrete processable objects
class Counter : public Processable {
private:
    int value_;
public:
    Counter(int v = 0) : value_(v) {}
    void process() override { ++value_; }
    int getValue() const override { return value_; }
    void setValue(int val) override { value_ = val; }
    std::string getType() const override { return "Counter"; }
};

class Accumulator : public Processable {
private:
    int sum_;
public:
    Accumulator(int s = 0) : sum_(s) {}
    void process() override { sum_ += 10; }
    int getValue() const override { return sum_; }
    void setValue(int val) override { sum_ = val; }
    std::string getType() const override { return "Accumulator"; }
};

// Advanced type-erased wrapper with multiple operations
class ProcessableObject {
private:
    struct ProcessableConcept {
        virtual ~ProcessableConcept() = default;
        virtual void process() = 0;
        virtual int getValue() const = 0;
        virtual void setValue(int val) = 0;
        virtual std::string getType() const = 0;
        virtual std::unique_ptr<ProcessableConcept> clone() const = 0;
    };

    template <typename T>
    struct ProcessableModel : ProcessableConcept {
        T object_;
        explicit ProcessableModel(T obj) : object_(std::move(obj)) {}

        void process() override { object_.process(); }
        int getValue() const override { return object_.getValue(); }
        void setValue(int val) override { object_.setValue(val); }
        std::string getType() const override { return object_.getType(); }

        std::unique_ptr<ProcessableConcept> clone() const override {
            return std::make_unique<ProcessableModel<T>>(object_);
        }
    };

    std::unique_ptr<ProcessableConcept> pimpl_;

public:
    template <typename T>
    ProcessableObject(T obj) : pimpl_(std::make_unique<ProcessableModel<T>>(std::move(obj))) {}

    ProcessableObject(const ProcessableObject& other) : pimpl_(other.pimpl_->clone()) {}
    ProcessableObject& operator=(const ProcessableObject& other) {
        pimpl_ = other.pimpl_->clone();
        return *this;
    }

    void process() { pimpl_->process(); }
    int getValue() const { return pimpl_->getValue(); }
    void setValue(int val) { pimpl_->setValue(val); }
    std::string getType() const { return pimpl_->getType(); }
};

// ============================================================================
// Example 4: Comparison with Traditional Inheritance
// ============================================================================

void demonstrateInheritance() {
    std::cout << "\n=== Traditional Inheritance Approach ===" << std::endl;

    // With inheritance, we need a common base class
    std::vector<std::unique_ptr<Drawable>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Square>(4.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0));

    for (const auto& shape : shapes) {
        std::cout << "Shape: " << shape->name() << " - ";
        shape->draw();
    }

    std::cout << "Problem: All objects must inherit from Drawable" << std::endl;
    std::cout << "Solution: Type erasure allows any type to be stored homogeneously" << std::endl;
}

void demonstrateTypeErasure() {
    std::cout << "\n=== Type Erasure Approach ===" << std::endl;
    std::cout << "Type erasure would allow storing different types without inheritance" << std::endl;
    std::cout << "Advantage: No inheritance requirement, any type works!" << std::endl;
}

#include "SampleRegistry.hpp"

void TypeErasureSample::run() {
    std::cout << "Running Type Erasure Sample..." << std::endl;

    // Demonstrate type erasure with callables
    std::cout << "\n=== Type Erasure with Callables (like std::function) ===" << std::endl;
    
    std::vector<AnyCallable> functions;
    
    // Add different types of callables
    functions.emplace_back([]() {
        std::cout << "Called from lambda!" << std::endl;
    });
    
    functions.emplace_back([]() {
        std::cout << "Called from function pointer!" << std::endl;
    });
    
    // Functor
    struct Functor {
        void operator()() const {
            std::cout << "Called from functor!" << std::endl;
        }
    };
    functions.emplace_back(Functor{});
    
    std::cout << "Calling all functions:" << std::endl;
    for (auto& func : functions) {
        func();
    }
    
    std::cout << "\nCopying and calling:" << std::endl;
    auto copy = functions[0];
    copy();
    
    // Demonstrate with inheritance approach for comparison
    std::cout << "\n=== Comparison: Inheritance vs Type Erasure ===" << std::endl;
    
    std::cout << "Traditional inheritance approach:" << std::endl;
    std::vector<std::unique_ptr<Drawable>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Square>(4.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0));
    
    for (const auto& shape : shapes) {
        std::cout << "Shape: " << shape->name() << " - ";
        shape->draw();
    }
    
    std::cout << "Problem: All objects must inherit from Drawable" << std::endl;
    
    // Show what type erasure enables
    std::cout << "\nType erasure enables storing different types without inheritance" << std::endl;
    std::cout << "(Full implementation would allow any drawable type)" << std::endl;

    std::cout << "\n=== Performance Considerations ===" << std::endl;
    std::cout << "Type Erasure Benefits:" << std::endl;
    std::cout << "- Homogeneous storage of heterogeneous types" << std::endl;
    std::cout << "- No inheritance requirements" << std::endl;
    std::cout << "- Runtime polymorphism without virtual inheritance" << std::endl;
    std::cout << "- Can work with third-party types" << std::endl;

    std::cout << "\nType Erasure Costs:" << std::endl;
    std::cout << "- Dynamic allocation (heap usage)" << std::endl;
    std::cout << "- Virtual function call overhead" << std::endl;
    std::cout << "- Copy operations require deep cloning" << std::endl;
    std::cout << "- Type information is 'erased' at compile time" << std::endl;

    std::cout << "\nType erasure demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(TypeErasureSample, "Type Erasure", 6);