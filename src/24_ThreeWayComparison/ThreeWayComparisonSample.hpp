#pragma once

#include "Testable.hpp"

class ThreeWayComparisonSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "24_ThreeWayComparison - C++20 Spaceship Operator (<=>)";
    }
};
