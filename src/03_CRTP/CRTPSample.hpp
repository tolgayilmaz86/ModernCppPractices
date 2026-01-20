#pragma once

#include "Testable.hpp"

class CRTPSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "03_CRTP - Curiously Recurring Template Pattern";
    }
};