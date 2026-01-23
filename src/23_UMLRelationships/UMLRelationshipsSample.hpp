#pragma once
#include "../Testable.hpp"
#include <string>

/**
 * @brief Demonstrates UML relationships in C++ code.
 * 
 * This sample shows how different UML class diagram relationships
 * are implemented in C++:
 * - Association (uses-a, knows-a)
 * - Aggregation (has-a, weak ownership)
 * - Composition (owns-a, strong ownership)
 * - Dependency (uses temporarily)
 * - Inheritance/Generalization (is-a)
 * - Realization/Implementation (implements interface)
 */
class UMLRelationshipsSample : public Testable {
public:
    void run() override;
    std::string name() const override { return "UML Relationships"; }
};
