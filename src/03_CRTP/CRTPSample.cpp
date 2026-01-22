#include "CRTPSample.hpp"
#include <iostream>
#include <memory>
#include <vector>

// CRTP Counter Mixin - tracks number of instances
template <typename T>
class Counter {
private:
    static size_t count;
public:
    Counter() { ++count; }
    Counter(const Counter&) { ++count; }
    ~Counter() { --count; }

    static size_t getCount() { return count; }
};

template <typename T>
size_t Counter<T>::count = 0;

// CRTP Comparison Mixin - provides comparison operators
template <typename T>
class Comparable {
public:
    friend bool operator==(const T& lhs, const T& rhs) {
        return !(lhs < rhs) && !(rhs < lhs);
    }

    friend bool operator!=(const T& lhs, const T& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<=(const T& lhs, const T& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    friend bool operator>(const T& lhs, const T& rhs) {
        return rhs < lhs;
    }

    friend bool operator>=(const T& lhs, const T& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }
};

// CRTP Clone Mixin - provides polymorphic copying
template <typename T>
class Cloneable {
public:
    std::unique_ptr<T> clone() const {
        return std::unique_ptr<T>(static_cast<T*>(doClone()));
    }

private:
    virtual Cloneable* doClone() const = 0;
};

// Abstract base class for shapes
class Shape {
protected:
    std::string name;
public:
    explicit Shape(std::string n) : name(std::move(n)) {}
    virtual ~Shape() = default;

    virtual double area() const = 0;
    const std::string& getName() const { return name; }
};

class Circle : public Shape, public Cloneable<Circle> {
private:
    double radius;
public:
    Circle(std::string name, double r) : Shape(std::move(name)), radius(r) {}

    double area() const override {
        return 3.14159 * radius * radius;
    }

private:
    Circle* doClone() const override {
        return new Circle(*this);
    }
};

class Rectangle : public Shape, public Cloneable<Rectangle> {
private:
    double width, height;
public:
    Rectangle(std::string name, double w, double h)
        : Shape(std::move(name)), width(w), height(h) {}

    double area() const override {
        return width * height;
    }

private:
    Rectangle* doClone() const override {
        return new Rectangle(*this);
    }
};

// Example classes using CRTP mixins
class Widget : public Counter<Widget>, public Comparable<Widget> {
private:
    int value;
public:
    explicit Widget(int v) : value(v) {}

    bool operator<(const Widget& other) const {
        return value < other.value;
    }

    int getValue() const { return value; }
};

#include "SampleRegistry.hpp"

void CRTPSample::run() {
    std::cout << "Running CRTP Sample..." << std::endl;

    // Counter mixin demonstration
    std::cout << "\n=== Counter Mixin ===" << std::endl;
    std::cout << "Initial Widget count: " << Widget::getCount() << std::endl;

    Widget w1(10);
    std::cout << "After creating w1: " << Widget::getCount() << std::endl;

    Widget w2(20);
    Widget w3(15);
    std::cout << "After creating w2 and w3: " << Widget::getCount() << std::endl;

    {
        Widget w4(5);
        std::cout << "Inside scope with w4: " << Widget::getCount() << std::endl;
    }
    std::cout << "After w4 goes out of scope: " << Widget::getCount() << std::endl;

    // Comparable mixin demonstration
    std::cout << "\n=== Comparable Mixin ===" << std::endl;
    std::cout << "w1 (10) < w2 (20): " << (w1 < w2) << std::endl;
    std::cout << "w1 (10) == w1 (10): " << (w1 == w1) << std::endl;
    std::cout << "w2 (20) > w3 (15): " << (w2 > w3) << std::endl;
    std::cout << "w1 (10) != w2 (20): " << (w1 != w2) << std::endl;

    // Cloneable mixin demonstration
    std::cout << "\n=== Cloneable Mixin ===" << std::endl;
    Circle circle("MyCircle", 5.0);
    Rectangle rect("MyRect", 4.0, 6.0);

    std::cout << "Original circle area: " << circle.area() << std::endl;
    std::cout << "Original rectangle area: " << rect.area() << std::endl;

    auto clonedCircle = circle.clone();
    auto clonedRect = rect.clone();

    std::cout << "Cloned circle area: " << clonedCircle->area() << std::endl;
    std::cout << "Cloned rectangle area: " << clonedRect->area() << std::endl;

    std::cout << "\nCRTP demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(CRTPSample, "CRTP", 3);