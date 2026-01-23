#pragma once

#include "Testable.hpp"

class InputOutputStreamSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "26_InputOutputStream - Extending iostream with Custom Types";
    }
};
