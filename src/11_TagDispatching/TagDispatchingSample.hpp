#pragma once

#include "../Testable.hpp"
#include <string>

class TagDispatchingSample : public Testable {
public:
    void run() override;
    std::string name() const override { return "Tag Dispatching"; }

private:
    // Demonstration functions
    void demonstrate_tag_dispatching();
    void demonstrate_overload_resolution();
    void demonstrate_best_practices();
};
