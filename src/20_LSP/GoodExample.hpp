#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>

namespace lsp_good {

// ============================================================================
// GOOD EXAMPLE 1: Proper Shape Hierarchy
// ============================================================================
// Don't model "is-a" relationships based on real-world taxonomy.
// Model based on BEHAVIORAL compatibility.

class Shape {
public:
    virtual ~Shape() = default;
    virtual int getArea() const = 0;
    virtual std::string name() const = 0;
};

class Rectangle : public Shape {
private:
    int width_, height_;
public:
    Rectangle(int width, int height) : width_(width), height_(height) {}
    
    void setWidth(int width) { width_ = width; }
    void setHeight(int height) { height_ = height; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
    int getArea() const override { return width_ * height_; }
    std::string name() const override { return "Rectangle"; }
};

// Square is NOT a subclass of Rectangle - separate hierarchy!
class Square : public Shape {
private:
    int side_;
public:
    explicit Square(int side) : side_(side) {}
    
    void setSide(int side) { side_ = side; }
    int getSide() const { return side_; }
    
    int getArea() const override { return side_ * side_; }
    std::string name() const override { return "Square"; }
};

// Now we can work with any Shape without surprises
void processShape(const Shape& shape) {
    std::cout << "    " << shape.name() << " area: " << shape.getArea() << std::endl;
}

// ============================================================================
// GOOD EXAMPLE 2: Proper Bird Hierarchy
// ============================================================================
// Separate flying from bird identity

class Bird {
public:
    virtual ~Bird() = default;
    virtual void eat() { std::cout << "    Bird is eating" << std::endl; }
    virtual std::string species() const = 0;
};

// Separate interface for flying capability
class Flyable {
public:
    virtual ~Flyable() = default;
    virtual void fly() = 0;
};

class Sparrow : public Bird, public Flyable {
public:
    std::string species() const override { return "Sparrow"; }
    void fly() override { 
        std::cout << "    Sparrow is flying" << std::endl; 
    }
};

class Eagle : public Bird, public Flyable {
public:
    std::string species() const override { return "Eagle"; }
    void fly() override { 
        std::cout << "    Eagle is soaring high" << std::endl; 
    }
};

// Penguin doesn't implement Flyable - no LSP violation!
class Penguin : public Bird {
public:
    std::string species() const override { return "Penguin"; }
    void swim() { 
        std::cout << "    Penguin is swimming" << std::endl; 
    }
};

// Safe to call fly() on any Flyable
void makeFlyablesFly(std::vector<Flyable*>& flyables) {
    for (auto* f : flyables) {
        f->fly();  // Always safe - all Flyables can fly!
    }
}

// ============================================================================
// GOOD EXAMPLE 3: Consistent Contracts
// ============================================================================

class DataReader {
public:
    virtual ~DataReader() = default;
    
    // Contract: Returns data or throws if unavailable
    virtual std::string read() = 0;
    
    // Let clients check availability first
    virtual bool hasData() const = 0;
};

class FileDataReader : public DataReader {
private:
    std::string content_;
    bool hasContent_;
public:
    FileDataReader(const std::string& content, bool hasContent) 
        : content_(content), hasContent_(hasContent) {}
    
    bool hasData() const override { return hasContent_; }
    
    std::string read() override {
        if (!hasContent_) {
            throw std::runtime_error("No data available");
        }
        return content_;
    }
};

class EmptyDataReader : public DataReader {
public:
    bool hasData() const override { return false; }
    
    std::string read() override {
        // Same behavior as base - throws when no data!
        throw std::runtime_error("No data available");
    }
};

// Client can check before reading
void processData(DataReader& reader) {
    if (reader.hasData()) {
        std::cout << "    Data: " << reader.read() << std::endl;
    } else {
        std::cout << "    No data available (checked safely)" << std::endl;
    }
}

// ============================================================================
// GOOD EXAMPLE 4: Composition over Inheritance
// ============================================================================

class WithdrawalPolicy {
public:
    virtual ~WithdrawalPolicy() = default;
    virtual bool canWithdraw(double balance, double amount) const = 0;
    virtual std::string description() const = 0;
};

class NoLimitPolicy : public WithdrawalPolicy {
public:
    bool canWithdraw(double balance, double amount) const override {
        return balance >= amount;
    }
    std::string description() const override { return "No limit"; }
};

class MinimumBalancePolicy : public WithdrawalPolicy {
    double minimum_;
public:
    explicit MinimumBalancePolicy(double min) : minimum_(min) {}
    bool canWithdraw(double balance, double amount) const override {
        return (balance - amount) >= minimum_;
    }
    std::string description() const override { 
        return "Min balance $" + std::to_string(static_cast<int>(minimum_)); 
    }
};

// Account uses composition - all accounts behave the same!
class Account {
private:
    double balance_;
    std::unique_ptr<WithdrawalPolicy> policy_;
public:
    Account(double balance, std::unique_ptr<WithdrawalPolicy> policy)
        : balance_(balance), policy_(std::move(policy)) {}

    bool withdraw(double amount) {
        if (amount <= 0 || !policy_->canWithdraw(balance_, amount)) {
            return false;
        }
        balance_ -= amount;
        return true;
    }

    double getBalance() const { return balance_; }
    std::string getPolicyDescription() const { return policy_->description(); }
};

// Any Account can be used interchangeably - same contract!
void processWithdrawal(Account& account, double amount) {
    if (account.withdraw(amount)) {
        std::cout << "    Withdrew $" << amount 
                  << ", Balance: $" << account.getBalance() << std::endl;
    } else {
        std::cout << "    Cannot withdraw $" << amount 
                  << " (Policy: " << account.getPolicyDescription() << ")" << std::endl;
    }
}

// Benefits of this design:
// 1. Shape: Rectangle and Square have independent behavior
// 2. Bird: Flyable interface separates concerns properly
// 3. DataReader: Consistent contract for all implementations
// 4. Account: Composition instead of problematic inheritance

} // namespace lsp_good
