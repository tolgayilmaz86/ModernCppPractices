#pragma once

#include "Testable.hpp"

class SRPSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "18_SRP - Single Responsibility Principle";
    }
};
