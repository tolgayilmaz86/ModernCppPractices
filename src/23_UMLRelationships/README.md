# UML Relationships in C++

## Overview

UML (Unified Modeling Language) class diagrams define several types of relationships between classes. Understanding these relationships is crucial for designing object-oriented systems. This sample demonstrates how each UML relationship translates to C++ code.

**Key Insight:** The relationship you choose affects object lifetimes, memory management, coupling, and code flexibility.

## UML Relationship Types

### Quick Reference

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  Relationship      │  UML Notation              │  Meaning                  │
├─────────────────────────────────────────────────────────────────────────────┤
│  Dependency        │  A - - - - - - -> B        │  A uses B temporarily     │
│  Association       │  A ──────────────> B       │  A knows about B          │
│  Aggregation       │  A ◇───────────── B        │  A has B (shared)         │
│  Composition       │  A ◆───────────── B        │  A owns B (exclusive)     │
│  Inheritance       │  A ─────────────▷ B        │  A is a type of B         │
│  Realization       │  A - - - - - - -▷ B        │  A implements B           │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 1. Dependency (Uses-a Temporarily)

### UML Notation
```
┌──────────┐         ┌──────────┐
│ Document │ - - - ->│ Printer  │
└──────────┘         └──────────┘
     uses temporarily
```
**Dashed line with open arrow** pointing to the dependency.

### Meaning
- **Weakest relationship**
- Class A temporarily uses class B
- No permanent connection between objects
- B appears as: parameter, local variable, or return type

### C++ Implementation
```cpp
class Document {
public:
    // Dependency: Printer passed as parameter
    void printTo(Printer& printer) {
        printer.print(content_);
    }
    
    // Dependency: Local instance
    void printWithNewPrinter() {
        Printer localPrinter;  // Local dependency
        localPrinter.print(content_);
    }
};
```

### When to Use
- Utility classes (loggers, formatters, validators)
- Factory methods
- Strategy pattern implementations

---

## 2. Association (Knows-a)

### UML Notation
```
┌──────────┐              ┌──────────┐
│ Student  │─────────────>│  Course  │
└──────────┘              └──────────┘
    knows about

Bidirectional:
┌──────────┐              ┌──────────┐
│ Student  │<────────────>│  Course  │
└──────────┘              └──────────┘
```
**Solid line** with optional arrow showing direction.

### Meaning
- Class A knows about class B
- Objects have **independent lifetimes**
- Neither owns the other
- Can be one-way or bidirectional

### C++ Implementation
```cpp
class Student {
private:
    std::vector<Course*> enrolled_courses_;  // Knows about courses
public:
    void enrollIn(Course* course) {
        enrolled_courses_.push_back(course);
    }
};

class Course {
private:
    std::vector<Student*> students_;  // Knows about students
};
```

### When to Use
- Peer objects that need to communicate
- Navigation between related objects
- When objects exist independently

---

## 3. Aggregation (Has-a, Weak Ownership)

### UML Notation
```
┌──────────┐              ┌──────────┐
│   Car    │◇────────────│  Engine  │
└──────────┘              └──────────┘
   has (but doesn't own)
```
**Empty (hollow) diamond** on the aggregate side.

### Meaning
- Class A **has** class B as a part
- B can exist independently of A
- B can be shared among multiple A's
- A does **not** manage B's lifetime

### C++ Implementation
```cpp
class Car {
private:
    Engine* engine_;  // Has engine, but doesn't own it
public:
    Car(Engine* engine) : engine_(engine) {}
    
    ~Car() {
        // Do NOT delete engine_ - we don't own it!
    }
    
    void setEngine(Engine* engine) { 
        engine_ = engine;  // Can be reassigned
    }
};

// Usage: Engine outlives Car
Engine v8("V8", 450);
{
    Car car(&v8);
}  // Car destroyed, engine still exists
v8.start();  // Still works!
```

### When to Use
- Parts that can be shared (e.g., team members in multiple projects)
- Parts that have independent lifecycle
- When reusing existing objects

---

## 4. Composition (Owns-a, Strong Ownership)

### UML Notation
```
┌──────────┐              ┌──────────┐
│  Human   │◆────────────│  Heart   │
└──────────┘              └──────────┘
   owns (controls lifetime)
```
**Filled (solid) diamond** on the composite side.

### Meaning
- Class A **owns** class B completely
- B **cannot exist** without A
- A **manages** B's entire lifecycle
- B is **not shared** between multiple A's

### C++ Implementation
```cpp
class Human {
private:
    std::unique_ptr<Heart> heart_;   // Owns heart
    std::unique_ptr<Brain> brain_;   // Owns brain
    
public:
    Human() 
        : heart_(std::make_unique<Heart>()),
          brain_(std::make_unique<Brain>()) {}
    
    // ~Human() automatically destroys heart_ and brain_
};

// Alternative: Direct member (also composition)
class Human {
private:
    Heart heart_;   // Direct member = composition
    Brain brain_;
};
```

### When to Use
- Parts that cannot exist without the whole
- Internal implementation details
- Value-type semantics for parts

---

## 5. Inheritance / Generalization (Is-a)

### UML Notation
```
┌──────────┐
│  Animal  │
└────▲─────┘
     │
     │ (hollow triangle)
     │
┌────┴─────┐
│   Dog    │
└──────────┘
```
**Solid line with hollow triangle** pointing to the base class.

### Meaning
- Class A **is a** specialized version of class B
- A inherits B's attributes and methods
- A can override B's virtual methods
- Enables **polymorphism**

### C++ Implementation
```cpp
class Animal {
protected:
    std::string name_;
public:
    virtual ~Animal() = default;
    virtual void speak() const { 
        std::cout << "Animal sound" << std::endl; 
    }
    void sleep() const { /* inherited behavior */ }
};

class Dog : public Animal {  // Dog IS-A Animal
public:
    void speak() const override {  // Override
        std::cout << "Woof!" << std::endl;
    }
    void fetch() { /* Dog-specific */ }
};

// Polymorphism
std::unique_ptr<Animal> pet = std::make_unique<Dog>();
pet->speak();  // "Woof!" - calls Dog's version
```

### When to Use
- True "is-a" relationships
- When you need polymorphic behavior
- Reusing and extending existing behavior

### Caution
- Prefer composition over inheritance when possible
- Follow Liskov Substitution Principle (LSP)
- Avoid deep inheritance hierarchies

---

## 6. Realization / Implementation (Implements Interface)

### UML Notation
```
    ┌─────────────┐
    │ «interface» │
    │  IDrawable  │
    └──────▲──────┘
           ┆
           ┆ (dashed + hollow triangle)
           ┆
    ┌──────┴──────┐
    │   Circle    │
    └─────────────┘
```
**Dashed line with hollow triangle** pointing to the interface.

### Meaning
- Class A **implements** interface B
- A provides concrete implementations of B's methods
- Enables **programming to interfaces**
- A class can implement **multiple interfaces**

### C++ Implementation
```cpp
// Interface (pure abstract class)
class IDrawable {
public:
    virtual ~IDrawable() = default;
    virtual void draw() const = 0;      // Pure virtual
    virtual void resize(double) = 0;    // Pure virtual
};

class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual std::string serialize() const = 0;
};

// Circle implements both interfaces
class Circle : public IDrawable, public ISerializable {
public:
    void draw() const override {
        std::cout << "Drawing circle" << std::endl;
    }
    void resize(double factor) override { radius_ *= factor; }
    std::string serialize() const override { return "Circle:" + ...; }
};

// Program to interface
void render(const IDrawable& shape) {
    shape.draw();  // Works with any IDrawable
}
```

### When to Use
- Defining contracts/capabilities
- Dependency injection
- Plugin architectures
- Multiple inheritance of behavior

---

## Relationship Strength Comparison

From **weakest** to **strongest**:

```
Dependency < Association < Aggregation < Composition < Inheritance
   ↓              ↓            ↓             ↓              ↓
temporary      knows       has (shared)   owns        is-a type of
```

### Decision Guide

| Question | If Yes | If No |
|----------|--------|-------|
| Does B need to exist after A is destroyed? | Aggregation/Association | Composition |
| Does A control B's lifetime? | Composition | Aggregation |
| Can B be shared among multiple A's? | Aggregation | Composition |
| Is A a specialized type of B? | Inheritance | No inheritance |
| Does A need to fulfill B's contract? | Realization | No interface |
| Is B only used temporarily in a method? | Dependency | Association+ |

---

## Comprehensive Example

Here's an e-commerce system using all relationship types:

```
                    ┌─────────────┐
                    │ «interface» │
                    │ INotifiable │
                    └──────▲──────┘
                           ┆ realizes
                    ┌──────┴──────┐
                    │  Customer   │
                    └──────▲──────┘
                           │ association
    ┌──────────────────────┼──────────────────────┐
    │                      │                      │
    │               ┌──────┴──────┐               │
    │               │    Order    │               │
    │               └─┬────────┬──┘               │
    │                 │        │                  │
    │   composition ◆│        │◇ aggregation     │
    │                 │        │                  │
    │          ┌──────┴──┐  ┌──┴───────┐          │
    │          │OrderItem│  │ Product  │          │
    │          └─────────┘  └──────────┘          │
    │                                             │
    │         dependency - - - - - - - - - - - - -│
    │                      ▼                      │
    │            ┌─────────────────┐              │
    │            │PaymentProcessor │              │
    │            └─────────────────┘              │
    └─────────────────────────────────────────────┘
```

```cpp
class Order {
private:
    Customer* customer_;                              // Association
    std::vector<std::unique_ptr<OrderItem>> items_;  // Composition
    
public:
    bool checkout(PaymentProcessor& processor) {     // Dependency
        if (processor.processPayment(total())) {
            customer_->notify("Order placed!");       // Via interface
            return true;
        }
        return false;
    }
};

class OrderItem {
    Product* product_;  // Association (knows product)
};

class Customer : public INotifiable {  // Realization
    void notify(const std::string& msg) override { /* ... */ }
};
```

---

## Best Practices

### 1. Prefer Composition Over Inheritance
```cpp
// Instead of: class Car : public Engine { }
// Do this:
class Car {
    std::unique_ptr<Engine> engine_;  // Composition
};
```

### 2. Use Interfaces for Flexibility
```cpp
class Service {
    ILogger* logger_;  // Easy to swap implementations
};
```

### 3. Make Ownership Clear
```cpp
// Composition: unique_ptr shows ownership
std::unique_ptr<Part> owned_part_;

// Aggregation: raw pointer or reference shows no ownership
Part* shared_part_;
```

### 4. Minimize Dependencies
```cpp
// Bad: Too many dependencies
class OrderService {
    Logger log_;
    EmailSender email_;
    SMSSender sms_;
    Database db_;
    // ...
};

// Better: Depend on interfaces, inject dependencies
class OrderService {
    ILogger& logger_;
    INotifier& notifier_;
    IRepository& repository_;
};
```

---

## Summary

| Relationship | UML Symbol | C++ Pattern | Lifetime |
|-------------|-----------|-------------|----------|
| **Dependency** | `- - - ->` | Parameter/Local | Independent |
| **Association** | `────────>` | Member pointer | Independent |
| **Aggregation** | `◇────────` | Member pointer (no delete) | Part survives |
| **Composition** | `◆────────` | `unique_ptr` / direct member | Part dies with whole |
| **Inheritance** | `────────▷` | `: public Base` | N/A |
| **Realization** | `- - - -▷` | `: public IInterface` | N/A |

Understanding these relationships helps you:
- Design cleaner architectures
- Manage object lifetimes correctly
- Reduce coupling between classes
- Make code more maintainable and testable
