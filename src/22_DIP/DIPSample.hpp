#pragma once

#include "Testable.hpp"

class DIPSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "22_DIP - Dependency Inversion Principle";
    }
};
