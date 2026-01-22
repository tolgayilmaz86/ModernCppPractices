#include "OCPSample.hpp"
#include "BadExample.hpp"
#include "GoodExample.hpp"
#include <iostream>
#include <memory>
#include <vector>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
namespace {

void demonstrate_bad_ocp() {
    std::cout << "\n=== BAD: Must Modify Code to Add Features ===" << std::endl;
    std::cout << "Adding a new shape requires changing AreaCalculator\n" << std::endl;

    ocp_bad::AreaCalculator calculator;
    
    std::vector<ocp_bad::Shape> shapes = {
        {ocp_bad::ShapeType::Circle, 5.0, 0},
        {ocp_bad::ShapeType::Rectangle, 4.0, 6.0},
        {ocp_bad::ShapeType::Triangle, 3.0, 4.0}
    };

    for (const auto& shape : shapes) {
        std::cout << "    Area: " << calculator.calculateArea(shape) << std::endl;
    }

    std::cout << "\n  Problems:" << std::endl;
    std::cout << "  - Want to add Pentagon? Modify AreaCalculator!" << std::endl;
    std::cout << "  - Want to add Hexagon? Modify AreaCalculator!" << std::endl;
    std::cout << "  - Switch statements grow forever" << std::endl;
    std::cout << "  - Easy to forget a case (runtime bugs)" << std::endl;
}

void demonstrate_good_ocp() {
    std::cout << "\n=== GOOD: Open for Extension, Closed for Modification ===" << std::endl;
    std::cout << "New shapes can be added WITHOUT changing AreaCalculator\n" << std::endl;

    ocp_good::AreaCalculator calculator;
    
    std::vector<std::unique_ptr<ocp_good::Shape>> shapes;
    shapes.push_back(std::make_unique<ocp_good::Circle>(5.0));
    shapes.push_back(std::make_unique<ocp_good::Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<ocp_good::Triangle>(3.0, 4.0));
    
    // NEW shapes added without modifying AreaCalculator!
    shapes.push_back(std::make_unique<ocp_good::Pentagon>(3.0));
    shapes.push_back(std::make_unique<ocp_good::Hexagon>(2.0));

    for (const auto& shape : shapes) {
        std::cout << "    " << shape->name() << " area: " << shape->area() << std::endl;
    }

    double total = calculator.calculateTotalArea(shapes);
    std::cout << "\n    Total area: " << total << std::endl;
}

void demonstrate_strategy_pattern() {
    std::cout << "\n=== Strategy Pattern (OCP in Action) ===" << std::endl;
    std::cout << "New discount types can be added without changing PriceCalculator\n" << std::endl;

    ocp_good::PriceCalculator calculator;
    double basePrice = 100.0;

    std::vector<std::unique_ptr<ocp_good::DiscountStrategy>> strategies;
    strategies.push_back(std::make_unique<ocp_good::NoDiscount>());
    strategies.push_back(std::make_unique<ocp_good::PercentageDiscount>(10));
    strategies.push_back(std::make_unique<ocp_good::PercentageDiscount>(20));
    strategies.push_back(std::make_unique<ocp_good::FixedDiscount>(15));
    strategies.push_back(std::make_unique<ocp_good::BuyOneGetOneDiscount>());

    std::cout << "    Base price: $" << basePrice << std::endl;
    std::cout << std::endl;

    for (const auto& strategy : strategies) {
        double finalPrice = calculator.calculateFinalPrice(basePrice, *strategy);
        std::cout << "    " << strategy->name() << ": $" << finalPrice << std::endl;
    }
}

void demonstrate_ocp_benefits() {
    std::cout << "\n=== Open/Closed Principle Benefits ===" << std::endl;

    std::cout << "\n  1. STABILITY" << std::endl;
    std::cout << "     - Existing code never changes" << std::endl;
    std::cout << "     - Tested code remains tested" << std::endl;

    std::cout << "\n  2. EXTENSIBILITY" << std::endl;
    std::cout << "     - New features = new classes" << std::endl;
    std::cout << "     - No risk to existing functionality" << std::endl;

    std::cout << "\n  3. MAINTAINABILITY" << std::endl;
    std::cout << "     - Changes are isolated" << std::endl;
    std::cout << "     - Easier to understand impact" << std::endl;

    std::cout << "\n  4. TESTABILITY" << std::endl;
    std::cout << "     - New classes need new tests only" << std::endl;
    std::cout << "     - Existing tests remain valid" << std::endl;
}

} // end anonymous namespace

#include "SampleRegistry.hpp"

void OCPSample::run() {
    std::cout << "Running Open/Closed Principle Sample..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "OCP: Software entities should be open for extension," << std::endl;
    std::cout << "     but closed for modification." << std::endl;

    demonstrate_bad_ocp();
    demonstrate_good_ocp();
    demonstrate_strategy_pattern();
    demonstrate_ocp_benefits();

    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "1. Use polymorphism instead of switch/if-else chains" << std::endl;
    std::cout << "2. New features should be new classes, not modified code" << std::endl;
    std::cout << "3. Strategy pattern is a common OCP implementation" << std::endl;
    std::cout << "4. Abstractions (interfaces) are key to OCP" << std::endl;

    std::cout << "\nOpen/Closed Principle demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(OCPSample, "Open/Closed Principle", 19);
