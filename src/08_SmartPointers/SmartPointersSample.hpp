#pragma once

#include "Testable.hpp"

class SmartPointersSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "08_SmartPointers - Smart Pointers";
    }
};