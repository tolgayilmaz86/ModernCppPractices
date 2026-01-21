#include "VariantVisitorSample.hpp"
#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <functional>

// ============================================================================
// Example 1: Basic Variant Usage
// ============================================================================

// Define a variant that can hold different shape types
using ShapeVariant = std::variant<std::monostate, double, int, std::string>;

// ============================================================================
// Example 2: Visitor Pattern with std::visit
// ============================================================================

// Custom visitor class for complex operations
class ShapeVisitor {
public:
    void operator()(std::monostate) const {
        std::cout << "Empty shape (monostate)" << std::endl;
    }

    void operator()(double radius) const {
        std::cout << "Processing circle with radius: " << radius
                  << ", area: " << (3.14159 * radius * radius) << std::endl;
    }

    void operator()(int sides) const {
        std::cout << "Processing polygon with " << sides << " sides" << std::endl;
    }

    void operator()(const std::string& name) const {
        std::cout << "Processing named shape: " << name << std::endl;
    }
};

// ============================================================================
// Example 3: Overloaded Lambdas (Simple Visitor)
// ============================================================================

// Helper for creating overloaded lambdas
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// ============================================================================
// Example 4: Advanced Visitor with State
// ============================================================================

class ShapeProcessor {
private:
    double total_area_ = 0.0;
    int shape_count_ = 0;

public:
    void process(const ShapeVariant& shape) {
        std::visit(*this, shape);
    }

    // Visitor operators
    void operator()(std::monostate) {
        std::cout << "Skipping empty shape" << std::endl;
    }

    void operator()(double radius) {
        double area = 3.14159 * radius * radius;
        total_area_ += area;
        shape_count_++;
        std::cout << "Circle area: " << area << std::endl;
    }

    void operator()(int sides) {
        // Approximate area for regular polygon
        double area = static_cast<double>(sides) * 10.0; // Simplified
        total_area_ += area;
        shape_count_++;
        std::cout << "Polygon area: " << area << std::endl;
    }

    void operator()(const std::string& name) {
        double area = 50.0; // Default area for named shapes
        total_area_ += area;
        shape_count_++;
        std::cout << "Named shape '" << name << "' area: " << area << std::endl;
    }

    void print_summary() const {
        std::cout << "\nSummary: " << shape_count_ << " shapes processed, "
                  << "total area: " << total_area_ << std::endl;
    }
};

// ============================================================================
// Example 5: Error Handling and Type Safety
// ============================================================================

class SafeShapeVisitor {
public:
    void operator()(std::monostate) const {
        throw std::runtime_error("Cannot process empty shape");
    }

    void operator()(double radius) const {
        if (radius <= 0) {
            throw std::invalid_argument("Radius must be positive");
        }
        std::cout << "Valid circle with radius: " << radius << std::endl;
    }

    void operator()(int sides) const {
        if (sides < 3) {
            throw std::invalid_argument("Polygon must have at least 3 sides");
        }
        std::cout << "Valid polygon with " << sides << " sides" << std::endl;
    }

    void operator()(const std::string& name) const {
        if (name.empty()) {
            throw std::invalid_argument("Shape name cannot be empty");
        }
        std::cout << "Valid named shape: " << name << std::endl;
    }
};

// ============================================================================
// Example 6: Comparison with Inheritance
// ============================================================================

// Traditional inheritance approach
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
    virtual double area() const = 0;
};

class Circle : public Shape {
    double radius_;
public:
    explicit Circle(double r) : radius_(r) {}
    void draw() const override { std::cout << "Drawing circle" << std::endl; }
    double area() const override { return 3.14159 * radius_ * radius_; }
};

class Polygon : public Shape {
    int sides_;
public:
    explicit Polygon(int s) : sides_(s) {}
    void draw() const override { std::cout << "Drawing polygon" << std::endl; }
    double area() const override { return static_cast<double>(sides_) * 10.0; }
};

void demonstrateInheritanceVariant() {
    std::cout << "\n=== Traditional Inheritance Approach ===" << std::endl;

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Polygon>(6));

    for (const auto& shape : shapes) {
        shape->draw();
        std::cout << "Area: " << shape->area() << std::endl;
    }

    std::cout << "Problem: Requires inheritance hierarchy" << std::endl;
    std::cout << "Problem: Cannot easily add new operations without modifying classes" << std::endl;
}

void demonstrateVariantVisitor() {
    std::cout << "\n=== Variant + Visitor Approach ===" << std::endl;

    std::vector<ShapeVariant> shapes = {5.0, 6, std::string("triangle")};

    ShapeVisitor visitor;
    for (const auto& shape : shapes) {
        std::visit(visitor, shape);
    }

    std::cout << "Advantage: No inheritance required" << std::endl;
    std::cout << "Advantage: Easy to add new operations (just new visitors)" << std::endl;
    std::cout << "Advantage: Type-safe at compile time" << std::endl;
}

// ============================================================================
// Example 7: Multiple Visitors on Same Data
// ============================================================================

void demonstrateMultipleVisitors() {
    std::cout << "\n=== Multiple Visitors on Same Data ===" << std::endl;

    std::vector<ShapeVariant> shapes = {3.0, 4, 5, std::string("square")};

    // Area calculator visitor
    auto area_visitor = overloaded{
        [](std::monostate) { return 0.0; },
        [](double r) { return 3.14159 * r * r; },
        [](int s) { return static_cast<double>(s) * 10.0; },
        [](const std::string&) { return 50.0; }
    };

    // Description visitor
    auto desc_visitor = overloaded{
        [](std::monostate) { return std::string("empty"); },
        [](double r) { return "circle(r=" + std::to_string(r) + ")"; },
        [](int s) { return "polygon(sides=" + std::to_string(s) + ")"; },
        [](const std::string& name) { return "named(" + name + ")"; }
    };

    double total_area = 0.0;
    for (const auto& shape : shapes) {
        std::string desc = std::visit(desc_visitor, shape);
        double area = std::visit(area_visitor, shape);
        total_area += area;
        std::cout << desc << " -> area: " << area << std::endl;
    }
    std::cout << "Total area: " << total_area << std::endl;
}

void VariantVisitorSample::run() {
    std::cout << "Running Variant Visitor Sample..." << std::endl;

    // Basic variant usage
    std::cout << "\n=== Basic Variant Usage ===" << std::endl;
    ShapeVariant v1 = 5.0;        // Circle with radius 5
    ShapeVariant v2 = 6;          // Polygon with 6 sides
    ShapeVariant v3 = "triangle"; // Named shape
    ShapeVariant v4;              // Empty (monostate)

    std::cout << "Variant sizes: " << sizeof(v1) << " bytes" << std::endl;

    // Custom visitor class
    std::cout << "\n=== Custom Visitor Class ===" << std::endl;
    ShapeVisitor visitor;
    std::visit(visitor, v1);
    std::visit(visitor, v2);
    std::visit(visitor, v3);
    std::visit(visitor, v4);

    // Overloaded lambdas
    std::cout << "\n=== Overloaded Lambdas (Simple Visitors) ===" << std::endl;
    auto simple_visitor = overloaded{
        [](std::monostate) { std::cout << "Empty shape" << std::endl; },
        [](double r) { std::cout << "Simple circle: " << r << std::endl; },
        [](int s) { std::cout << "Simple polygon: " << s << " sides" << std::endl; },
        [](const std::string& n) { std::cout << "Simple named: " << n << std::endl; }
    };

    std::visit(simple_visitor, v1);
    std::visit(simple_visitor, v2);

    // Advanced visitor with state
    std::cout << "\n=== Advanced Visitor with State ===" << std::endl;
    ShapeProcessor processor;
    std::vector<ShapeVariant> shapes = {2.5, 4, 8, std::string("hexagon")};

    for (const auto& shape : shapes) {
        processor.process(shape);
    }
    processor.print_summary();

    // Error handling
    std::cout << "\n=== Error Handling ===" << std::endl;
    try {
        ShapeVariant invalid_shape = 0;  // Invalid radius
        std::visit(SafeShapeVisitor{}, invalid_shape);
    } catch (const std::exception& e) {
        std::cout << "Caught error: " << e.what() << std::endl;
    }

    try {
        ShapeVariant valid_shape = 3.0;
        std::visit(SafeShapeVisitor{}, valid_shape);
    } catch (const std::exception& e) {
        std::cout << "Unexpected error: " << e.what() << std::endl;
    }

    // Comparison with inheritance
    demonstrateInheritanceVariant();
    demonstrateVariantVisitor();

    // Multiple visitors
    demonstrateMultipleVisitors();

    std::cout << "\n=== Performance Characteristics ===" << std::endl;
    std::cout << "Variant + Visitor Benefits:" << std::endl;
    std::cout << "- No inheritance hierarchy required" << std::endl;
    std::cout << "- Type-safe at compile time" << std::endl;
    std::cout << "- Easy to add new operations (new visitors)" << std::endl;
    std::cout << "- Better performance than inheritance (no vtable)" << std::endl;
    std::cout << "- Fixed memory layout" << std::endl;

    std::cout << "\nVariant + Visitor Costs:" << std::endl;
    std::cout << "- Must know all types at compile time" << std::endl;
    std::cout << "- Cannot easily add new types without recompilation" << std::endl;
    std::cout << "- std::visit has some overhead" << std::endl;
    std::cout << "- More complex for simple cases" << std::endl;

    std::cout << "\nWhen to use Variant + Visitor:" << std::endl;
    std::cout << "- Fixed set of types known at compile time" << std::endl;
    std::cout << "- Need type-safe operations on discriminated unions" << std::endl;
    std::cout << "- Want to avoid inheritance hierarchies" << std::endl;
    std::cout << "- Performance-critical code with small type sets" << std::endl;

    std::cout << "\nVariant visitor demonstration completed!" << std::endl;
}