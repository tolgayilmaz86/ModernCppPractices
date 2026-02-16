#pragma once

#include "../Testable.hpp"
#include <string>

class RTTISample : public Testable {
public:
    void run() override;
    std::string name() const override { return "RTTI (Run-Time Type Information)"; }

private:
    void demonstrate_typeid();
    void demonstrate_dynamic_cast();
    void demonstrate_type_info();
    void demonstrate_rtti_limitations();
    void demonstrate_cpp17_any();
    void demonstrate_cpp20_features();
    void demonstrate_best_practices();

    // Helper classes for demonstration
    class Shape;
    class Circle;
    class Square;
    class Triangle;
};
