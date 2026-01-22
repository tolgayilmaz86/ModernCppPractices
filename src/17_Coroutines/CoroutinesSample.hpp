#pragma once

#include "Testable.hpp"

class CoroutinesSample : public Testable {
public:
    void run() override;
    std::string name() const override {
        return "17_Coroutines - C++20 Coroutines";
    }
};
