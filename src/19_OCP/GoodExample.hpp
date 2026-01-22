#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <cmath>

namespace ocp_good {

// ============================================================================
// GOOD EXAMPLE: Open for extension, closed for modification
// ============================================================================
// New shapes can be added WITHOUT modifying existing code.
// The AreaCalculator works with any shape that implements the interface.

// Abstract base class defines the contract
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};

// Each shape implements its own area calculation
class Circle : public Shape {
private:
    double radius_;
public:
    explicit Circle(double radius) : radius_(radius) {}
    
    double area() const override {
        return 3.14159 * radius_ * radius_;
    }
    
    std::string name() const override { return "Circle"; }
};

class Rectangle : public Shape {
private:
    double width_, height_;
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}
    
    double area() const override {
        return width_ * height_;
    }
    
    std::string name() const override { return "Rectangle"; }
};

class Triangle : public Shape {
private:
    double base_, height_;
public:
    Triangle(double base, double height) : base_(base), height_(height) {}
    
    double area() const override {
        return 0.5 * base_ * height_;
    }
    
    std::string name() const override { return "Triangle"; }
};

// NEW SHAPES can be added WITHOUT modifying any existing code!
class Pentagon : public Shape {
private:
    double side_;
public:
    explicit Pentagon(double side) : side_(side) {}
    
    double area() const override {
        // Area = (1/4) * sqrt(5(5+2*sqrt(5))) * s^2
        return 0.25 * std::sqrt(5 * (5 + 2 * std::sqrt(5))) * side_ * side_;
    }
    
    std::string name() const override { return "Pentagon"; }
};

class Hexagon : public Shape {
private:
    double side_;
public:
    explicit Hexagon(double side) : side_(side) {}
    
    double area() const override {
        // Area = (3 * sqrt(3) / 2) * s^2
        return (3 * std::sqrt(3) / 2) * side_ * side_;
    }
    
    std::string name() const override { return "Hexagon"; }
};

// AreaCalculator is CLOSED for modification - never needs to change!
class AreaCalculator {
public:
    double calculateArea(const Shape& shape) {
        return shape.area();  // Delegates to the shape
    }

    double calculateTotalArea(const std::vector<std::unique_ptr<Shape>>& shapes) {
        double total = 0;
        for (const auto& shape : shapes) {
            total += shape->area();
        }
        return total;
    }
};

// ============================================================================
// Strategy Pattern Example (another OCP application)
// ============================================================================

// Discount strategy interface
class DiscountStrategy {
public:
    virtual ~DiscountStrategy() = default;
    virtual double calculateDiscount(double amount) const = 0;
    virtual std::string name() const = 0;
};

class NoDiscount : public DiscountStrategy {
public:
    double calculateDiscount(double) const override { return 0; }
    std::string name() const override { return "No Discount"; }
};

class PercentageDiscount : public DiscountStrategy {
private:
    double percentage_;
public:
    explicit PercentageDiscount(double percentage) : percentage_(percentage) {}
    double calculateDiscount(double amount) const override {
        return amount * percentage_ / 100.0;
    }
    std::string name() const override { 
        return std::to_string(static_cast<int>(percentage_)) + "% Discount"; 
    }
};

class FixedDiscount : public DiscountStrategy {
private:
    double fixedAmount_;
public:
    explicit FixedDiscount(double amount) : fixedAmount_(amount) {}
    double calculateDiscount(double amount) const override {
        return std::min(fixedAmount_, amount);
    }
    std::string name() const override { 
        return "Fixed $" + std::to_string(static_cast<int>(fixedAmount_)) + " Discount"; 
    }
};

// NEW: Buy-one-get-one discount - added without modifying PriceCalculator!
class BuyOneGetOneDiscount : public DiscountStrategy {
public:
    double calculateDiscount(double amount) const override {
        return amount * 0.5;  // 50% off (BOGO)
    }
    std::string name() const override { return "Buy One Get One"; }
};

// PriceCalculator is CLOSED for modification
class PriceCalculator {
public:
    double calculateFinalPrice(double basePrice, const DiscountStrategy& strategy) {
        double discount = strategy.calculateDiscount(basePrice);
        return basePrice - discount;
    }
};

// Benefits of this design:
// 1. Adding new shapes doesn't modify AreaCalculator
// 2. Adding new discounts doesn't modify PriceCalculator
// 3. Existing code remains stable and tested
// 4. New features are added by extension, not modification
// 5. Follows Open/Closed Principle

} // namespace ocp_good
