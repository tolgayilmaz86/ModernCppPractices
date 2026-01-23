#pragma once

#include "../Testable.hpp"
#include <string>

class CastingTypesSample : public Testable {
public:
    void run() override;
    std::string name() const override { return "C++ Cast Types"; }

private:
    void demonstrate_static_cast();
    void demonstrate_dynamic_cast();
    void demonstrate_const_cast();
    void demonstrate_reinterpret_cast();
    void demonstrate_bit_cast();
    void demonstrate_best_practices();
    void demonstrate_casting_best_practices();

    // Helper classes for demonstration
    class Animal;
    class Dog;
    class Cat;
};
