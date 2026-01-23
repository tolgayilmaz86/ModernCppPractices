#pragma once

#include "Testable.hpp"

class ProjectionsSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "25_Projections - C++20 Range Projections";
    }
};
