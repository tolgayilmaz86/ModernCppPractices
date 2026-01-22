# Open/Closed Principle (OCP)

## Overview

The Open/Closed Principle states that **software entities (classes, modules, functions) should be open for extension, but closed for modification**.

> "You should be able to extend a class's behavior without modifying it." — Robert C. Martin

## The Problem

When you need to modify existing code every time you add a new feature:
- **Risk of bugs** — Changing tested code can introduce regressions
- **Merge conflicts** — Multiple developers touching the same file
- **Growing complexity** — Switch statements and if-else chains grow unbounded
- **Harder testing** — Must re-test existing functionality

## Bad Example ❌

```cpp
class AreaCalculator {
public:
    double calculateArea(const Shape& shape) {
        switch (shape.type) {
            case ShapeType::Circle:
                return 3.14159 * shape.radius * shape.radius;
            case ShapeType::Rectangle:
                return shape.width * shape.height;
            case ShapeType::Triangle:
                return 0.5 * shape.base * shape.height;
            // Adding Pentagon? MODIFY THIS FILE!
            // Adding Hexagon? MODIFY THIS FILE!
            // The switch grows forever...
        }
    }
};
```

**Problems:**
- Every new shape requires modifying `AreaCalculator`
- Switch statement grows with each shape
- Easy to forget a case
- Existing tests might break

## Good Example ✅

```cpp
// Abstract interface
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
};

// Each shape knows its own area
class Circle : public Shape {
    double radius_;
public:
    double area() const override {
        return 3.14159 * radius_ * radius_;
    }
};

class Rectangle : public Shape {
    double width_, height_;
public:
    double area() const override {
        return width_ * height_;
    }
};

// NEW: Pentagon - no modification to existing code!
class Pentagon : public Shape {
    double side_;
public:
    double area() const override {
        return 1.72 * side_ * side_;
    }
};

// AreaCalculator NEVER changes!
class AreaCalculator {
public:
    double calculateArea(const Shape& shape) {
        return shape.area();  // Delegates to shape
    }
};
```

**Benefits:**
- Adding Pentagon = adding a new file
- AreaCalculator stays untouched
- Existing tests remain valid
- No risk to existing functionality

## Strategy Pattern (OCP in Action)

The Strategy pattern is a perfect example of OCP:

```cpp
// Strategy interface
class DiscountStrategy {
public:
    virtual double calculate(double amount) const = 0;
};

class PercentageDiscount : public DiscountStrategy {
    double percent_;
public:
    double calculate(double amount) const override {
        return amount * percent_ / 100;
    }
};

class FixedDiscount : public DiscountStrategy {
    double fixed_;
public:
    double calculate(double amount) const override {
        return std::min(fixed_, amount);
    }
};

// PriceCalculator is CLOSED for modification
class PriceCalculator {
public:
    double finalPrice(double base, const DiscountStrategy& strategy) {
        return base - strategy.calculate(base);
    }
};

// NEW: Seasonal discount - no changes to PriceCalculator!
class SeasonalDiscount : public DiscountStrategy { /* ... */ };
```

## How to Achieve OCP

| Technique | Description |
|-----------|-------------|
| **Polymorphism** | Use virtual functions and inheritance |
| **Strategy Pattern** | Inject behavior via interfaces |
| **Template Method** | Define skeleton, let subclasses fill in |
| **Decorator Pattern** | Wrap objects to add behavior |
| **Plugin Architecture** | Load new functionality at runtime |

## Identifying OCP Violations

| Red Flag | Example |
|----------|---------|
| `switch` on type | `switch (shape.type)` |
| `if-else` chains on type | `if (customer.isVIP()) ... else if ...` |
| Modifying class for new features | Adding method for each new export format |
| "God class" that knows all types | `PaymentProcessor` with every payment method |

## OCP vs Other Patterns

| Pattern | How it Relates to OCP |
|---------|----------------------|
| **Strategy** | Swap algorithms without modifying client |
| **Decorator** | Add behavior without modifying original |
| **Factory** | Create new types without modifying creation code |
| **Observer** | Add subscribers without modifying publisher |

## Real-World Examples

| Before (Violates OCP) | After (Follows OCP) |
|----------------------|---------------------|
| `switch(paymentType)` in checkout | `PaymentStrategy` interface |
| `if (format == "pdf") else if (format == "csv")` | `ExportStrategy` interface |
| Logger with hardcoded destinations | `LogDestination` interface |
| Report with hardcoded calculations | `ReportMetric` interface |

## Sample Output

```
Running Open/Closed Principle Sample...

=== BAD: Must Modify Code to Add Features ===
Adding a new shape requires changing AreaCalculator

    Area: 78.5397
    Area: 24
    Area: 6

  Problems:
  - Want to add Pentagon? Modify AreaCalculator!
  - Want to add Hexagon? Modify AreaCalculator!

=== GOOD: Open for Extension, Closed for Modification ===
New shapes can be added WITHOUT changing AreaCalculator

    Circle area: 78.5397
    Rectangle area: 24
    Triangle area: 6
    Pentagon area: 15.4849
    Hexagon area: 10.3923

    Total area: 134.417

=== Strategy Pattern (OCP in Action) ===
New discount types without changing PriceCalculator

    Base price: $100

    No Discount: $100
    10% Discount: $90
    20% Discount: $80
    Fixed $15 Discount: $85
    Buy One Get One: $50
```

## Key Takeaways

1. **Closed for modification** = Don't change existing code
2. **Open for extension** = Add new behavior via new classes
3. **Use abstractions** = Interfaces enable extensibility
4. **Avoid switch on types** = Use polymorphism instead
5. **New feature = new class** = Not modified old class

## Further Reading

- [Strategy Pattern](https://refactoring.guru/design-patterns/strategy)
- [Clean Architecture by Robert C. Martin](https://www.amazon.com/Clean-Architecture-Craftsmans-Software-Structure/dp/0134494164)
