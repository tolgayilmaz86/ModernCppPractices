#pragma once

#include "../Testable.hpp"
#include <string>

class CopyAndSwapIdiomSample : public Testable {
public:
    void run() override;
    std::string name() const override { return "Copy and Swap Idiom"; }

private:
    void demonstrate_copy_and_swap();
    void demonstrate_exception_safety();
    void demonstrate_best_practices();

    // Helper class demonstrating the idiom
    class Resource;
};
