#pragma once

#include "Testable.hpp"

class OCPSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "19_OCP - Open/Closed Principle";
    }
};
