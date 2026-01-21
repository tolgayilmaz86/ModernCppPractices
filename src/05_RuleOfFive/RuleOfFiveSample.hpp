#pragma once

#include "Testable.hpp"

class RuleOfFiveSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "05_RuleOfFive - Rule of Five";
    }
};