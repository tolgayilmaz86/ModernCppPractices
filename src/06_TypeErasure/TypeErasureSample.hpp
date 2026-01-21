#pragma once

#include "Testable.hpp"

class TypeErasureSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "06_TypeErasure - Type Erasure";
    }
};