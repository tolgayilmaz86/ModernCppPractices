#pragma once

#include "Testable.hpp"

class ConceptsSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "16_Concepts - C++20 Concepts and Constraints";
    }
};
