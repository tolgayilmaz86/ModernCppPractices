#pragma once

#include "Testable.hpp"

class PimplSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "04_PIMPL - Pointer to Implementation";
    }
};