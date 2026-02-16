#include "RTTISample.hpp"
#include <iostream>
#include <typeinfo>
#include <memory>
#include <vector>
#include <any>
#include <string_view>

// Helper classes for demonstration
class RTTISample::Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
    
    // C++20: We can use constexpr virtual functions in some contexts
    virtual int sides() const = 0;
};

class RTTISample::Circle : public Shape {
    double radius_;
public:
    explicit Circle(double r) : radius_(r) {}
    double area() const override { return 3.14159 * radius_ * radius_; }
    std::string name() const override { return "Circle"; }
    int sides() const override { return 0; } // Circle has no sides
    double radius() const { return radius_; }
};

class RTTISample::Square : public Shape {
    double side_;
public:
    explicit Square(double s) : side_(s) {}
    double area() const override { return side_ * side_; }
    std::string name() const override { return "Square"; }
    int sides() const override { return 4; }
    double side() const { return side_; }
};

class RTTISample::Triangle : public Shape {
    double base_, height_;
public:
    Triangle(double b, double h) : base_(b), height_(h) {}
    double area() const override { return 0.5 * base_ * height_; }
    std::string name() const override { return "Triangle"; }
    int sides() const override { return 3; }
};

void RTTISample::demonstrate_typeid() {
    std::cout << "\n=== typeid Operator ===\n";
    
    // Basic typeid usage
    int i = 42;
    double d = 3.14;
    const char* str = "Hello";
    
    std::cout << "typeid(int).name(): " << typeid(int).name() << "\n";
    std::cout << "typeid(double).name(): " << typeid(double).name() << "\n";
    std::cout << "typeid(const char*).name(): " << typeid(const char*).name() << "\n";
    
    // typeid on variables
    std::cout << "\ntypeid(i).name(): " << typeid(i).name() << "\n";
    std::cout << "typeid(d).name(): " << typeid(d).name() << "\n";
    std::cout << "typeid(str).name(): " << typeid(str).name() << "\n";
    
    // typeid with polymorphic types (runtime check)
    Shape* shape = new Circle(5.0);
    std::cout << "\nPolymorphic typeid:\n";
    std::cout << "Static type (Shape*): " << typeid(Shape*).name() << "\n";
    std::cout << "Dynamic type (*shape): " << typeid(*shape).name() << "\n";
    
    // Comparing type_info objects
    Circle circle(2.0);
    Square square(3.0);
    Shape& shapeRef1 = circle;
    Shape& shapeRef2 = square;
    
    std::cout << "\nComparing types:\n";
    std::cout << "shapeRef1 is Circle: " << (typeid(shapeRef1) == typeid(Circle) ? "Yes" : "No") << "\n";
    std::cout << "shapeRef2 is Circle: " << (typeid(shapeRef2) == typeid(Circle) ? "Yes" : "No") << "\n";
    std::cout << "shapeRef1 == shapeRef2: " << (typeid(shapeRef1) == typeid(shapeRef2) ? "Yes" : "No") << "\n";
    
    delete shape;
}

void RTTISample::demonstrate_dynamic_cast() {
    std::cout << "\n=== dynamic_cast Operator ===\n";
    
    // Create a collection of shapes
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(2.0));
    shapes.push_back(std::make_unique<Square>(3.0));
    shapes.push_back(std::make_unique<Triangle>(4.0, 5.0));
    shapes.push_back(std::make_unique<Circle>(1.5));
    
    for (const auto& shape : shapes) {
        std::cout << "\nProcessing " << shape->name() << ":\n";
        std::cout << "  Area: " << shape->area() << "\n";
        
        // Try to cast to Circle
        if (Circle* circle = dynamic_cast<Circle*>(shape.get())) {
            std::cout << "  Successfully cast to Circle!\n";
            std::cout << "  Radius: " << circle->radius() << "\n";
        }
        
        // Try to cast to Square
        if (Square* square = dynamic_cast<Square*>(shape.get())) {
            std::cout << "  Successfully cast to Square!\n";
            std::cout << "  Side: " << square->side() << "\n";
        }
        
        // Try to cast to Triangle
        if (Triangle* triangle = dynamic_cast<Triangle*>(shape.get())) {
            std::cout << "  Successfully cast to Triangle!\n";
            std::cout << "  Sides: " << triangle->sides() << "\n";
        }
    }
    
    // dynamic_cast with references (throws on failure)
    std::cout << "\n--- dynamic_cast with references ---\n";
    Shape& shapeRef = *shapes[0]; // This is a Circle
    
    try {
        Circle& circleRef = dynamic_cast<Circle&>(shapeRef);
        std::cout << "Reference cast to Circle successful!\n";
        std::cout << "Radius: " << circleRef.radius() << "\n";
    } catch (const std::bad_cast& e) {
        std::cout << "bad_cast exception: " << e.what() << "\n";
    }
    
    // This will throw because shapes[1] is a Square, not a Circle
    try {
        Shape& squareRef = *shapes[1];
        [[maybe_unused]] Circle& circleRef = dynamic_cast<Circle&>(squareRef);
        std::cout << "This should not print!\n";
    } catch (const std::bad_cast& e) {
        std::cout << "Expected bad_cast for Square->Circle: " << e.what() << "\n";
    }
}

void RTTISample::demonstrate_type_info() {
    std::cout << "\n=== std::type_info Details ===\n";
    
    const std::type_info& intInfo = typeid(int);
    const std::type_info& circleInfo = typeid(Circle);
    
    // type_info member functions
    std::cout << "int type name: " << intInfo.name() << "\n";
    std::cout << "Circle type name: " << circleInfo.name() << "\n";
    
    // Before C++20: hash_code() (available since C++11)
    std::cout << "\nhash_code() for int: " << intInfo.hash_code() << "\n";
    std::cout << "hash_code() for Circle: " << circleInfo.hash_code() << "\n";
    
    // type_info comparison
    std::cout << "\ntype_info comparisons:\n";
    std::cout << "int == double: " << (typeid(int) == typeid(double) ? "true" : "false") << "\n";
    std::cout << "int == int: " << (typeid(int) == typeid(int) ? "true" : "false") << "\n";
    std::cout << "int != double: " << (typeid(int) != typeid(double) ? "true" : "false") << "\n";
    
    // type_info before() - implementation-defined ordering
    std::cout << "\ntype_info::before() (implementation-defined ordering):\n";
    std::cout << "int before double: " << (typeid(int).before(typeid(double)) ? "true" : "false") << "\n";
}

void RTTISample::demonstrate_rtti_limitations() {
    std::cout << "\n=== RTTI Limitations and Considerations ===\n";
    
    std::cout << "1. RTTI only works with polymorphic classes (at least one virtual function)\n";
    std::cout << "   Non-polymorphic types use static (compile-time) type info\n";
    
    // Non-polymorphic class example
    struct NonPolymorphic {
        int value;
    };
    
    NonPolymorphic np;
    std::cout << "\n   NonPolymorphic type: " << typeid(NonPolymorphic).name() << "\n";
    std::cout << "   typeid(np) gives static type: " << typeid(np).name() << "\n";
    
    std::cout << "\n2. RTTI can be disabled with compiler flags (-fno-rtti on GCC/Clang)\n";
    std::cout << "   This is common in embedded systems to reduce binary size\n";
    
    std::cout << "\n3. RTTI has runtime overhead:\n";
    std::cout << "   - Type information stored in vtable\n";
    std::cout << "   - String comparisons for type names\n";
    std::cout << "   - dynamic_cast traverses inheritance hierarchy\n";
    
    std::cout << "\n4. type_info::name() is implementation-defined:\n";
    std::cout << "   - May be mangled (GCC/Clang) or readable (MSVC)\n";
    std::cout << "   - Not portable across compilers\n";
    
    std::cout << "\n5. RTTI doesn't work across shared library boundaries reliably\n";
}

void RTTISample::demonstrate_cpp17_any() {
    std::cout << "\n=== C++17: std::any and RTTI ===\n";
    
    // std::any uses RTTI internally for type checking
    std::cout << "std::any stores any CopyConstructible type:\n";
    
    std::any a1 = 42;
    std::any a2 = 3.14;
    std::any a3 = std::string("Hello");
    std::any a4 = Circle(2.5);
    
    std::cout << "a1 type: " << a1.type().name() << "\n";
    std::cout << "a2 type: " << a2.type().name() << "\n";
    std::cout << "a3 type: " << a3.type().name() << "\n";
    std::cout << "a4 type: " << a4.type().name() << "\n";
    
    // std::any_cast uses RTTI for type checking
    std::cout << "\nstd::any_cast with type checking:\n";
    
    try {
        int value = std::any_cast<int>(a1);
        std::cout << "a1 as int: " << value << "\n";
    } catch (const std::bad_any_cast& e) {
        std::cout << "bad_any_cast: " << e.what() << "\n";
    }
    
    try {
        // This will throw - a1 is int, not double
        double value = std::any_cast<double>(a1);
        std::cout << "a1 as double: " << value << "\n";
    } catch (const std::bad_any_cast& e) {
        std::cout << "Expected bad_any_cast (int != double): " << e.what() << "\n";
    }
    
    // Pointer version returns nullptr on failure
    if (int* ptr = std::any_cast<int>(&a1)) {
        std::cout << "Pointer access successful: " << *ptr << "\n";
    }
    
    if (double* ptr = std::any_cast<double>(&a1)) {
        std::cout << "This should not print\n";
    } else {
        std::cout << "Pointer access returned nullptr (correct)\n";
    }
    
    // has_value() check
    std::any empty;
    std::cout << "\nempty.any.has_value(): " << (empty.has_value() ? "true" : "false") << "\n";
    std::cout << "a1.has_value(): " << (a1.has_value() ? "true" : "false") << "\n";
    
    std::cout << "\nNote: std::any is built on RTTI - disabling RTTI disables std::any\n";
}

void RTTISample::demonstrate_cpp20_features() {
    std::cout << "\n=== C++20: RTTI-Related Features ===\n";
    
    // C++20: std::type_identity (for type deduction)
    std::cout << "C++20 std::type_identity:\n";
    std::cout << "  Useful for preventing template argument deduction\n";
    std::cout << "  Example: void foo(std::type_identity_t<T> arg)\n";
    
    // C++20: consteval can be used with type checking at compile time
    std::cout << "\nC++20 consteval with type info:\n";
    std::cout << "  Compile-time type checking is possible with constexpr/consteval\n";
    
    // C++20: Concepts provide compile-time type constraints
    std::cout << "\nC++20 Concepts as RTTI alternative:\n";
    std::cout << "  Concepts check types at compile-time instead of runtime\n";
    std::cout << "  This is more efficient and catches errors earlier\n";
    
    // Example: Using concepts instead of RTTI
    auto processCircle = [](const Circle& c) {
        std::cout << "  Processing circle with radius: " << c.radius() << "\n";
    };
    
    Circle c(3.0);
    processCircle(c);
    
    std::cout << "\nC++20 std::is_constant_evaluated():\n";
    std::cout << "  Can detect if code is running at compile-time\n";
    std::cout << "  Useful for optimizing type-dependent code\n";
    
    // C++20: source_location (not directly RTTI but related)
    std::cout << "\nC++20 std::source_location:\n";
    std::cout << "  Provides compile-time source location info\n";
    std::cout << "  Can be used for debugging without RTTI overhead\n";
    
    // Comparison table
    std::cout << "\n=== RTTI Evolution Across C++ Standards ===\n";
    std::cout << "+---------+------------------------------------------+\n";
    std::cout << "| C++14   | Basic RTTI (typeid, dynamic_cast)        |\n";
    std::cout << "| C++17   | + std::any (uses RTTI internally)        |\n";
    std::cout << "| C++20   | + Concepts (compile-time alternative)    |\n";
    std::cout << "|         | + std::type_identity                     |\n";
    std::cout << "|         | + std::source_location                   |\n";
    std::cout << "+---------+------------------------------------------+\n";
}

void RTTISample::demonstrate_best_practices() {
    std::cout << "\n=== RTTI Best Practices ===\n";
    
    std::cout << "1. Prefer virtual functions over explicit type checking\n";
    std::cout << "   BAD: if (typeid(obj) == typeid(Derived)) { ... }\n";
    std::cout << "   GOOD: obj.virtualFunction();\n";
    
    std::cout << "\n2. Use dynamic_cast for safe downcasting when needed\n";
    std::cout << "   - Always check the result (nullptr for pointers)\n";
    std::cout << "   - Catch std::bad_cast for references\n";
    
    std::cout << "\n3. Consider alternatives to RTTI:\n";
    std::cout << "   - Virtual functions (polymorphism)\n";
    std::cout << "   - Visitor pattern\n";
    std::cout << "   - std::variant with std::visit (C++17)\n";
    std::cout << "   - Concepts (C++20) for compile-time checks\n";
    
    std::cout << "\n4. When to use RTTI:\n";
    std::cout << "   - Debugging and logging\n";
    std::cout << "   - Serialization frameworks\n";
    std::cout << "   - Plugin systems\n";
    std::cout << "   - When virtual functions aren't sufficient\n";
    
    std::cout << "\n5. Performance considerations:\n";
    std::cout << "   - typeid is relatively cheap (just vtable lookup)\n";
    std::cout << "   - dynamic_cast can be expensive (hierarchy traversal)\n";
    std::cout << "   - Cache results if used frequently\n";
    
    // Example: Visitor pattern as alternative
    std::cout << "\n=== Alternative: Visitor Pattern (No RTTI) ===\n";
    std::cout << "Instead of:\n";
    std::cout << "  if (auto* circle = dynamic_cast<Circle*>(shape)) { ... }\n";
    std::cout << "  else if (auto* square = dynamic_cast<Square*>(shape)) { ... }\n";
    std::cout << "\nUse visitor pattern:\n";
    std::cout << "  shape->accept(visitor);  // Double dispatch\n";
    
    // Example: std::variant alternative
    std::cout << "\n=== Alternative: std::variant (C++17) ===\n";
    std::cout << "std::variant<Circle, Square, Triangle> shape;\n";
    std::cout << "std::visit([](auto& s) { s.area(); }, shape);\n";
    std::cout << "No RTTI needed - all type-safe at compile time!\n";
}

#include "SampleRegistry.hpp"

void RTTISample::run() {
    std::cout << "Running RTTI (Run-Time Type Information) Sample...\n";
    
    demonstrate_typeid();
    demonstrate_dynamic_cast();
    demonstrate_type_info();
    demonstrate_rtti_limitations();
    demonstrate_cpp17_any();
    demonstrate_cpp20_features();
    demonstrate_best_practices();
    
    std::cout << "\n=== RTTI Summary ===\n";
    std::cout << "RTTI provides runtime type information for polymorphic types.\n";
    std::cout << "Key components:\n";
    std::cout << "  - typeid: Get type information at runtime\n";
    std::cout << "  - dynamic_cast: Safe downcasting with runtime check\n";
    std::cout << "  - std::type_info: Type information class\n";
    std::cout << "\nC++17 additions:\n";
    std::cout << "  - std::any: Type-safe container using RTTI\n";
    std::cout << "\nC++20 additions:\n";
    std::cout << "  - Concepts: Compile-time type constraints (RTTI alternative)\n";
    std::cout << "  - std::type_identity: Type deduction control\n";
    std::cout << "\nUse RTTI judiciously - prefer compile-time solutions when possible!\n";
    
    std::cout << "\nRTTI demonstration completed!\n";
}

// Auto-register this sample
REGISTER_SAMPLE(RTTISample, "RTTI (Run-Time Type Information)", 27);
