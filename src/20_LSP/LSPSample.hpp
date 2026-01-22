#pragma once

#include "Testable.hpp"

class LSPSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "20_LSP - Liskov Substitution Principle";
    }
};
