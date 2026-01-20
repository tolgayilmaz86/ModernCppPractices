#pragma once

#include "../Testable.hpp"

class RAIISample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "01_RAII - Resource Acquisition Is Initialization";
    }
};