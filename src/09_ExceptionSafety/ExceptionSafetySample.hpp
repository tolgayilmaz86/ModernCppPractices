#pragma once

#include "Testable.hpp"

class ExceptionSafetySample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "09_ExceptionSafety - Exception Safety";
    }
};