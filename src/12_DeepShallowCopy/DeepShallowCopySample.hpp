#pragma once

#include "../Testable.hpp"
#include <string>

class DeepShallowCopySample : public Testable {
public:
    void run() override;
    std::string name() const override { return "Deep vs Shallow Copy"; }

private:
    void demonstrate_shallow_copy();
    void demonstrate_deep_copy();
    void demonstrate_rule_of_three_five();
    void demonstrate_best_practices();

    // Helper classes
    class ShallowCopyResource;
    class DeepCopyResource;
};
