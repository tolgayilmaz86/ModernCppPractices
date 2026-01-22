#pragma once

#include "Testable.hpp"

class ISPSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "21_ISP - Interface Segregation Principle";
    }
};
