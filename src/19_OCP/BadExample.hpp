#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace ocp_bad {

// ============================================================================
// BAD EXAMPLE: Must modify class to add new behavior
// ============================================================================
// Every time we add a new shape, we must modify the AreaCalculator class.
// This violates OCP: the class is not "closed for modification".

enum class ShapeType {
    Circle,
    Rectangle,
    Triangle
    // Adding a new shape? Must modify this enum AND AreaCalculator!
};

struct Shape {
    ShapeType type;
    double dimension1;  // radius for circle, width for rectangle, base for triangle
    double dimension2;  // 0 for circle, height for rectangle and triangle
};

class AreaCalculator {
public:
    // This method must be modified every time we add a new shape!
    double calculateArea(const Shape& shape) {
        switch (shape.type) {
            case ShapeType::Circle:
                return 3.14159 * shape.dimension1 * shape.dimension1;
            case ShapeType::Rectangle:
                return shape.dimension1 * shape.dimension2;
            case ShapeType::Triangle:
                return 0.5 * shape.dimension1 * shape.dimension2;
            // NEW SHAPE? Add another case here!
            // What if we forget? Runtime bug!
            default:
                std::cout << "    [BAD] Unknown shape type!" << std::endl;
                return 0;
        }
    }

    double calculateTotalArea(const std::vector<Shape>& shapes) {
        double total = 0;
        for (const auto& shape : shapes) {
            total += calculateArea(shape);
        }
        return total;
    }
};

// Problems with this design:
// 1. Adding Pentagon requires modifying AreaCalculator
// 2. Adding Hexagon requires modifying AreaCalculator
// 3. Switch statements grow unbounded
// 4. Easy to forget a case
// 5. Violates Open/Closed Principle

class DiscountCalculator {
public:
    // Another example of OCP violation
    double calculateDiscount(const std::string& customerType, double amount) {
        if (customerType == "regular") {
            return amount * 0.0;  // no discount
        } else if (customerType == "premium") {
            return amount * 0.1;  // 10% discount
        } else if (customerType == "vip") {
            return amount * 0.2;  // 20% discount
        }
        // New customer type? Modify this method!
        return 0;
    }
};

} // namespace ocp_bad
