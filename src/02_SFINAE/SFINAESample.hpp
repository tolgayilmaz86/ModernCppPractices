#pragma once

#include "Testable.hpp"

class SFINAESample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "02_SFINAE - Substitution Failure Is Not An Error";
    }
};