#include "VariantVisitorSample.hpp"
#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <type_traits>
#include <memory>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
// with similarly named classes in other translation units
namespace {

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
// overloaded keyword helps to combine multiple lambdas into one
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

// ============================================================================
// Example 8: Direct Access with std::get and std::holds_alternative
// ============================================================================

void demonstrateDirectAccess() {
    std::cout << "\n=== Direct Access with std::get and std::holds_alternative ===\n";

    ShapeVariant shape = 5.0;  // Circle with radius 5

    // Check what type is stored
    if (std::holds_alternative<double>(shape)) {
        std::cout << "Shape holds a double (circle radius): " << std::get<double>(shape) << std::endl;
    }

    // Safe access with checking
    shape = 6;  // Polygon with 6 sides
    if (std::holds_alternative<int>(shape)) {
        int sides = std::get<int>(shape);
        std::cout << "Shape holds an int (polygon sides): " << sides << std::endl;
    }

    // Unsafe access - will throw std::bad_variant_access if wrong type
    try {
        shape = std::string("triangle");
        // This will throw because shape holds std::string, not double
        double radius = std::get<double>(shape);
        std::cout << "Radius: " << radius << std::endl;
    } catch (const std::bad_variant_access& e) {
        std::cout << "Caught std::bad_variant_access: " << e.what() << std::endl;
        std::cout << "This is expected - we tried to get double from string variant" << std::endl;
    }

    // Safe access with std::get_if (C++17)
    shape = 4.5;
    if (auto radius_ptr = std::get_if<double>(&shape)) {
        std::cout << "Safe access with get_if - radius: " << *radius_ptr << std::endl;
    } else {
        std::cout << "Shape doesn't hold a double" << std::endl;
    }
}

// ============================================================================
// Example 9: Memory Layout and Performance
// ============================================================================

void demonstrateMemoryLayout() {
    std::cout << "\n=== Memory Layout and Performance ===\n";

    using SmallVariant = std::variant<char, short>;  // Small types
    using LargeVariant = std::variant<std::string, std::vector<int>>;  // Large types

    std::cout << "sizeof(std::variant<char, short>): " << sizeof(SmallVariant) << " bytes" << std::endl;
    std::cout << "sizeof(std::variant<std::string, std::vector<int>>): " << sizeof(LargeVariant) << " bytes" << std::endl;
    std::cout << "sizeof(std::string): " << sizeof(std::string) << " bytes" << std::endl;
    std::cout << "sizeof(std::vector<int>): " << sizeof(std::vector<int>) << " bytes" << std::endl;

    std::cout << "\nVariant size is max(alternatives) + index overhead" << std::endl;
    std::cout << "Small variants are efficient, large variants may waste space" << std::endl;
}

// ============================================================================
// Example 10: Advanced Lambda Visitors
// ============================================================================

void demonstrateAdvancedLambdas() {
    std::cout << "\n=== Advanced Lambda Visitors ===\n";

    std::vector<ShapeVariant> shapes = {3.0, 4, std::string("pentagon"), std::monostate{}};

    // Generic visitor that works with any callable
    auto generic_visitor = [](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
            return "empty";
        } else if constexpr (std::is_same_v<T, double>) {
            return "circle(r=" + std::to_string(value) + ")";
        } else if constexpr (std::is_same_v<T, int>) {
            return "polygon(sides=" + std::to_string(value) + ")";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "named(" + value + ")";
        } else {
            return "unknown";
        }
    };

    for (const auto& shape : shapes) {
        std::string desc = std::visit(generic_visitor, shape);
        std::cout << "Shape: " << desc << std::endl;
    }

    std::cout << "\nAdvanced lambdas can use constexpr and type traits for complex logic" << std::endl;
}

// ============================================================================
// Example 11: Variant vs Inheritance Performance Comparison
// ============================================================================

void demonstratePerformanceComparison() {
    std::cout << "\n=== Performance Comparison: Variant vs Inheritance ===\n";

    const int iterations = 100000;

    // Variant approach
    using FastVariant = std::variant<double, int>;  // Only value types
    std::vector<FastVariant> variant_shapes;
    for (int i = 0; i < iterations; ++i) {
        variant_shapes.push_back(i % 2 == 0 ? FastVariant{i * 1.0} : FastVariant{i});
    }

    auto area_visitor = overloaded{
        [](double r) -> double { return 3.14159 * r * r; },
        [](int s) -> double { return s * 10.0; }
    };

    auto start = std::chrono::high_resolution_clock::now();
    double total_area = 0.0;
    for (const auto& shape : variant_shapes) {
        total_area += std::visit(area_visitor, shape);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto variant_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Variant approach: " << variant_time.count() << " microseconds" << std::endl;
    std::cout << "Total area calculated: " << total_area << std::endl;

    // Note: Full inheritance comparison would require heap allocation
    // This demonstrates the value semantics advantage
    std::cout << "\nKey advantages of variants:" << std::endl;
    std::cout << "- No heap allocation for small objects" << std::endl;
    std::cout << "- Better cache locality" << std::endl;
    std::cout << "- No virtual function overhead" << std::endl;
    std::cout << "- Compile-time polymorphism resolution" << std::endl;
}

} // end anonymous namespace

#include "SampleRegistry.hpp"

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
    // demonstrateInheritanceVariant();  // Disabled due to heap allocation issue
    demonstrateVariantVisitor();

    // Multiple visitors
    demonstrateMultipleVisitors();

    // Direct access methods
    demonstrateDirectAccess();

    // Memory layout
    demonstrateMemoryLayout();

    // Advanced lambdas
    demonstrateAdvancedLambdas();

    // Performance comparison
    demonstratePerformanceComparison();

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

// Auto-register this sample
REGISTER_SAMPLE(VariantVisitorSample, "Variant Visitor", 7);