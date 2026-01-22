#include "LSPSample.hpp"
#include "BadExample.hpp"
#include "GoodExample.hpp"
#include <iostream>
#include <memory>
#include <vector>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
namespace {

void demonstrate_bad_rectangle_square() {
    std::cout << "\n=== BAD: Rectangle/Square Violation ===" << std::endl;
    std::cout << "Square inherits from Rectangle but breaks its contract\n" << std::endl;

    lsp_bad::Rectangle rect(5, 5);
    std::cout << "    Rectangle 5x5:" << std::endl;
    lsp_bad::processRectangle(rect);

    lsp_bad::Square square(5);
    std::cout << "\n    Square 5x5 (treated as Rectangle):" << std::endl;
    lsp_bad::processRectangle(square);  // LSP violation!
}

void demonstrate_bad_bird() {
    std::cout << "\n=== BAD: Penguin Can't Fly ===" << std::endl;
    std::cout << "Penguin inherits from Bird but throws on fly()\n" << std::endl;

    lsp_bad::Bird sparrow;
    std::cout << "    Sparrow:" << std::endl;
    lsp_bad::makeBirdFly(sparrow);

    lsp_bad::Penguin penguin;
    std::cout << "\n    Penguin (treated as Bird):" << std::endl;
    try {
        lsp_bad::makeBirdFly(penguin);  // Throws!
    } catch (const std::runtime_error& e) {
        std::cout << "    [BAD] Exception: " << e.what() << std::endl;
    }
}

void demonstrate_good_shapes() {
    std::cout << "\n=== GOOD: Proper Shape Hierarchy ===" << std::endl;
    std::cout << "Rectangle and Square are separate classes\n" << std::endl;

    lsp_good::Rectangle rect(4, 6);
    lsp_good::Square square(5);

    lsp_good::processShape(rect);
    lsp_good::processShape(square);

    // No surprises - each shape has its own interface
    rect.setWidth(10);
    rect.setHeight(3);
    std::cout << "    Rectangle after resize: " << rect.getArea() << std::endl;

    square.setSide(7);
    std::cout << "    Square after resize: " << square.getArea() << std::endl;
}

void demonstrate_good_birds() {
    std::cout << "\n=== GOOD: Separate Flyable Interface ===" << std::endl;
    std::cout << "Only birds that can fly implement Flyable\n" << std::endl;

    lsp_good::Sparrow sparrow;
    lsp_good::Eagle eagle;
    lsp_good::Penguin penguin;

    std::cout << "  All birds can eat:" << std::endl;
    sparrow.eat();
    eagle.eat();
    penguin.eat();

    std::cout << "\n  Only Flyables can fly:" << std::endl;
    std::vector<lsp_good::Flyable*> flyables = {&sparrow, &eagle};
    lsp_good::makeFlyablesFly(flyables);

    std::cout << "\n  Penguin swims instead:" << std::endl;
    penguin.swim();
}

void demonstrate_good_data_reader() {
    std::cout << "\n=== GOOD: Consistent Data Reader Contract ===" << std::endl;
    std::cout << "All readers follow the same contract\n" << std::endl;

    lsp_good::FileDataReader fileReader("Hello World", true);
    lsp_good::EmptyDataReader emptyReader;

    lsp_good::processData(fileReader);
    lsp_good::processData(emptyReader);
}

void demonstrate_good_accounts() {
    std::cout << "\n=== GOOD: Composition Over Inheritance ===" << std::endl;
    std::cout << "Accounts use policy objects instead of inheritance\n" << std::endl;

    lsp_good::Account checking(100, std::make_unique<lsp_good::NoLimitPolicy>());
    lsp_good::Account savings(100, std::make_unique<lsp_good::MinimumBalancePolicy>(50));

    std::cout << "  Checking account (no limit policy):" << std::endl;
    lsp_good::processWithdrawal(checking, 80);
    lsp_good::processWithdrawal(checking, 30);  // Will fail (insufficient funds)

    std::cout << "\n  Savings account (min balance $50):" << std::endl;
    lsp_good::processWithdrawal(savings, 40);
    lsp_good::processWithdrawal(savings, 20);  // Will fail (min balance)
}

void demonstrate_lsp_rules() {
    std::cout << "\n=== Liskov Substitution Principle Rules ===" << std::endl;

    std::cout << "\n  1. SIGNATURE RULES" << std::endl;
    std::cout << "     - Return types can be more specific (covariance)" << std::endl;
    std::cout << "     - Parameter types can be more general (contravariance)" << std::endl;
    std::cout << "     - No new exceptions (that base doesn't throw)" << std::endl;

    std::cout << "\n  2. BEHAVIORAL RULES" << std::endl;
    std::cout << "     - Preconditions cannot be strengthened" << std::endl;
    std::cout << "     - Postconditions cannot be weakened" << std::endl;
    std::cout << "     - Invariants must be preserved" << std::endl;

    std::cout << "\n  3. DESIGN GUIDELINES" << std::endl;
    std::cout << "     - Model behavior, not real-world taxonomy" << std::endl;
    std::cout << "     - Use interfaces to separate capabilities" << std::endl;
    std::cout << "     - Prefer composition over inheritance" << std::endl;
}

} // end anonymous namespace

#include "SampleRegistry.hpp"

void LSPSample::run() {
    std::cout << "Running Liskov Substitution Principle Sample..." << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "LSP: Objects of a superclass should be replaceable" << std::endl;
    std::cout << "     with objects of its subclasses without breaking." << std::endl;

    demonstrate_bad_rectangle_square();
    demonstrate_bad_bird();
    demonstrate_good_shapes();
    demonstrate_good_birds();
    demonstrate_good_data_reader();
    demonstrate_good_accounts();
    demonstrate_lsp_rules();

    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "1. Subtypes must honor the base type's contract" << std::endl;
    std::cout << "2. Don't model based on real-world 'is-a' relationships" << std::endl;
    std::cout << "3. Use interfaces to separate capabilities" << std::endl;
    std::cout << "4. Prefer composition when inheritance is problematic" << std::endl;

    std::cout << "\nLiskov Substitution Principle demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(LSPSample, "Liskov Substitution Principle", 20);
