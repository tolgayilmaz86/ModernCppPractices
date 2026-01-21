#pragma once

#include "Testable.hpp"

class VariantVisitorSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "07_VariantVisitor - Variant and Visitor Pattern";
    }
};