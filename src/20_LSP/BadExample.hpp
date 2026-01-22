#pragma once

#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace lsp_bad {

// ============================================================================
// BAD EXAMPLE 1: Classic Rectangle/Square Violation
// ============================================================================
// A Square is-a Rectangle mathematically, but substituting Square for Rectangle
// breaks client code expectations.

class Rectangle {
protected:
    int width_;
    int height_;

public:
    Rectangle(int width, int height) : width_(width), height_(height) {}
    virtual ~Rectangle() = default;

    virtual void setWidth(int width) { width_ = width; }
    virtual void setHeight(int height) { height_ = height; }

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getArea() const { return width_ * height_; }
};

// Square violates LSP: changing width must also change height
class Square : public Rectangle {
public:
    explicit Square(int side) : Rectangle(side, side) {}

    // These overrides BREAK the Rectangle contract!
    void setWidth(int width) override {
        width_ = width;
        height_ = width;  // Unexpected side effect!
    }

    void setHeight(int height) override {
        width_ = height;  // Unexpected side effect!
        height_ = height;
    }
};

// This function expects Rectangle behavior, but Square breaks it!
void processRectangle(Rectangle& rect) {
    int originalWidth = rect.getWidth();
    rect.setHeight(10);
    
    // This assertion should hold for a Rectangle:
    // Setting height shouldn't change width!
    if (rect.getWidth() != originalWidth) {
        std::cout << "    [BAD] LSP VIOLATED! Setting height changed width!" << std::endl;
        std::cout << "    Expected width: " << originalWidth << ", Got: " << rect.getWidth() << std::endl;
    }
}

// ============================================================================
// BAD EXAMPLE 2: Exception-throwing subclass
// ============================================================================

class Bird {
public:
    virtual ~Bird() = default;
    virtual void fly() {
        std::cout << "    Bird is flying" << std::endl;
    }
};

// Penguin can't fly - throws exception, violating LSP!
class Penguin : public Bird {
public:
    void fly() override {
        throw std::runtime_error("Penguins can't fly!");  // LSP violation!
    }
};

void makeBirdFly(Bird& bird) {
    // Client expects all birds to fly - Penguin breaks this!
    bird.fly();
}

// ============================================================================
// BAD EXAMPLE 3: Weakened postconditions
// ============================================================================

class FileReader {
public:
    virtual ~FileReader() = default;
    
    virtual std::string read() {
        // Postcondition: returns non-empty content or throws
        return "file content";
    }
};

class EmptyFileReader : public FileReader {
public:
    std::string read() override {
        // WEAKENS postcondition by returning empty string
        // instead of throwing when no content
        return "";  // Client may not handle this!
    }
};

// ============================================================================
// BAD EXAMPLE 4: Strengthened preconditions
// ============================================================================

class Account {
protected:
    double balance_;
public:
    explicit Account(double balance) : balance_(balance) {}
    virtual ~Account() = default;

    virtual void withdraw(double amount) {
        // Precondition: amount > 0
        if (amount <= 0) {
            throw std::invalid_argument("Amount must be positive");
        }
        balance_ -= amount;
    }

    double getBalance() const { return balance_; }
};

class SavingsAccount : public Account {
    double minimumBalance_;
public:
    SavingsAccount(double balance, double minBalance) 
        : Account(balance), minimumBalance_(minBalance) {}

    void withdraw(double amount) override {
        // STRENGTHENS precondition - adds new requirement!
        if (balance_ - amount < minimumBalance_) {
            throw std::runtime_error("Cannot go below minimum balance!");
        }
        Account::withdraw(amount);
    }
};

// Problems with this design:
// 1. Square: Setting one dimension changes both (unexpected behavior)
// 2. Penguin: Throws exception instead of flying (breaks contract)
// 3. EmptyFileReader: Returns empty instead of throwing (weakens postcondition)
// 4. SavingsAccount: Adds new precondition (restricts valid inputs)

} // namespace lsp_bad
