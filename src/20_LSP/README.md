# Liskov Substitution Principle (LSP)

## Overview

The Liskov Substitution Principle states that **objects of a superclass should be replaceable with objects of its subclasses without altering the correctness of the program**.

> "If S is a subtype of T, then objects of type T may be replaced with objects of type S without altering any of the desirable properties of the program." — Barbara Liskov

## The Problem

When subclasses don't properly substitute for their parent:
- **Runtime surprises** — Code that worked with base class breaks with derived
- **Exception bombs** — Derived class throws unexpected exceptions
- **Contract violations** — Preconditions/postconditions don't match
- **Semantic errors** — Code appears to work but produces wrong results

## Bad Example 1: Rectangle/Square ❌

```cpp
class Rectangle {
public:
    virtual void setWidth(int w) { width_ = w; }
    virtual void setHeight(int h) { height_ = h; }
    int getArea() const { return width_ * height_; }
};

class Square : public Rectangle {
public:
    // VIOLATION: Changes both dimensions!
    void setWidth(int w) override { 
        width_ = w; 
        height_ = w;  // Unexpected side effect!
    }
    void setHeight(int h) override { 
        width_ = h;   // Unexpected side effect!
        height_ = h; 
    }
};

void processRectangle(Rectangle& r) {
    r.setWidth(5);
    r.setHeight(10);
    // Expected: area = 50
    // With Square: area = 100 (WRONG!)
}
```

**Why it's wrong:** Setting height shouldn't change width. Square breaks this expectation.

## Bad Example 2: Bird/Penguin ❌

```cpp
class Bird {
public:
    virtual void fly() { /* flying code */ }
};

class Penguin : public Bird {
public:
    void fly() override {
        throw std::runtime_error("Can't fly!");  // LSP violation!
    }
};

void makeBirdsFly(std::vector<Bird*>& birds) {
    for (auto* bird : birds) {
        bird->fly();  // Boom! Penguin throws!
    }
}
```

**Why it's wrong:** Client code expects all Birds to fly. Penguin breaks this expectation.

## Good Example 1: Proper Shape Hierarchy ✅

```cpp
class Shape {
public:
    virtual int getArea() const = 0;
};

class Rectangle : public Shape {
    int width_, height_;
public:
    void setWidth(int w) { width_ = w; }
    void setHeight(int h) { height_ = h; }
    int getArea() const override { return width_ * height_; }
};

// Square is NOT a Rectangle - separate class!
class Square : public Shape {
    int side_;
public:
    void setSide(int s) { side_ = s; }
    int getArea() const override { return side_ * side_; }
};
```

**Why it's right:** Both shapes fulfill the `Shape` contract. No unexpected behaviors.

## Good Example 2: Separate Flyable Interface ✅

```cpp
class Bird {
public:
    virtual void eat() = 0;
};

class Flyable {
public:
    virtual void fly() = 0;
};

class Sparrow : public Bird, public Flyable {
    void eat() override { /* ... */ }
    void fly() override { /* ... */ }
};

class Penguin : public Bird {
    void eat() override { /* ... */ }
    // Doesn't implement Flyable - no violation!
};

void makeFlyablesFly(std::vector<Flyable*>& f) {
    for (auto* bird : f) {
        bird->fly();  // Always safe!
    }
}
```

**Why it's right:** Only things that can fly implement `Flyable`. No exceptions.

## LSP Rules

### Contract Rules

| Rule | Description | Violation Example |
|------|-------------|-------------------|
| **Preconditions** | Cannot be strengthened | Base accepts any amount, derived requires min $10 |
| **Postconditions** | Cannot be weakened | Base guarantees non-empty result, derived returns empty |
| **Invariants** | Must be preserved | Base maintains sorted order, derived doesn't |

### Signature Rules

| Rule | Description |
|------|-------------|
| **Return types** | Can be more specific (covariant) |
| **Parameters** | Can be more general (contravariant) |
| **Exceptions** | Cannot throw new exception types |

## How to Fix LSP Violations

| Problem | Solution |
|---------|----------|
| Square/Rectangle | Don't inherit, use common `Shape` interface |
| Penguin/Bird | Separate `Flyable` interface |
| Derived throws new exception | Design interface to avoid exception need |
| Derived adds precondition | Use composition with policy objects |
| Derived weakens postcondition | Maintain same guarantees |

## Composition Over Inheritance

When inheritance creates LSP issues, use composition:

```cpp
class WithdrawalPolicy {
public:
    virtual bool canWithdraw(double balance, double amount) = 0;
};

class NoLimitPolicy : public WithdrawalPolicy {
    bool canWithdraw(double balance, double amount) override {
        return balance >= amount;
    }
};

class MinBalancePolicy : public WithdrawalPolicy {
    double minimum_;
public:
    bool canWithdraw(double balance, double amount) override {
        return (balance - amount) >= minimum_;
    }
};

// All accounts behave the same - policy handles differences!
class Account {
    WithdrawalPolicy* policy_;
public:
    bool withdraw(double amount) {
        if (policy_->canWithdraw(balance_, amount)) {
            balance_ -= amount;
            return true;
        }
        return false;  // Never throws - consistent behavior!
    }
};
```

## Sample Output

```
Running Liskov Substitution Principle Sample...

=== BAD: Rectangle/Square Violation ===
    Rectangle 5x5:
    Square 5x5 (treated as Rectangle):
    [BAD] LSP VIOLATED! Setting height changed width!
    Expected width: 5, Got: 10

=== BAD: Penguin Can't Fly ===
    Sparrow:
    Bird is flying
    Penguin (treated as Bird):
    [BAD] Exception: Penguins can't fly!

=== GOOD: Proper Shape Hierarchy ===
    Rectangle area: 24
    Square area: 25

=== GOOD: Separate Flyable Interface ===
  Only Flyables can fly:
    Sparrow is flying
    Eagle is soaring high
  Penguin swims instead:
    Penguin is swimming

=== GOOD: Composition Over Inheritance ===
  Checking account (no limit policy):
    Withdrew $80, Balance: $20
  Savings account (min balance $50):
    Withdrew $40, Balance: $60
    Cannot withdraw $20 (Policy: Min balance $50)
```

## LSP Checklist

✅ Can I substitute any subclass for the base class?  
✅ Does the subclass maintain all invariants?  
✅ Are all preconditions the same or weaker?  
✅ Are all postconditions the same or stronger?  
✅ No new exceptions thrown?  
✅ No unexpected side effects?

## Key Takeaways

1. **Model behavior, not taxonomy** — Square is-a Rectangle mathematically, not programmatically
2. **Subtypes must honor contracts** — Same preconditions, postconditions, invariants
3. **Separate capabilities into interfaces** — Flyable, Swimmable, etc.
4. **Prefer composition** — When inheritance creates substitution problems
5. **Test with polymorphism** — If code breaks when you substitute, there's an LSP violation

## Further Reading

- [Liskov Substitution Principle (Wikipedia)](https://en.wikipedia.org/wiki/Liskov_substitution_principle)
- [A Behavioral Notion of Subtyping (Original Paper)](https://www.cs.cmu.edu/~wing/publications/LiskovWing94.pdf)
