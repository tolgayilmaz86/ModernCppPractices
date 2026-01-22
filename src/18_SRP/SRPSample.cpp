#include "SRPSample.hpp"
#include "BadExample.hpp"
#include "GoodExample.hpp"
#include <iostream>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
namespace {

void demonstrate_bad_srp() {
    std::cout << "\n=== BAD: Class with Multiple Responsibilities ===" << std::endl;
    std::cout << "The User class handles: data, validation, persistence, email, logging\n" << std::endl;

    srp_bad::User user("Alice", "alice@example.com", 30);
    
    // All these unrelated operations are in one class
    user.setName("Alice Smith");
    user.validateEmail("invalid-email");  // Validation in User class?
    user.formatWelcomeEmail();            // Email formatting in User class?
    
    std::cout << "\n  Problems:" << std::endl;
    std::cout << "  - Can't test validation without creating a User" << std::endl;
    std::cout << "  - Can't reuse email templates elsewhere" << std::endl;
    std::cout << "  - Changing logging affects User class" << std::endl;
    std::cout << "  - Multiple reasons to modify this class" << std::endl;
}

void demonstrate_good_srp() {
    std::cout << "\n=== GOOD: Single Responsibility per Class ===" << std::endl;
    std::cout << "Each class has ONE job:\n" << std::endl;

    // Each responsibility is separate
    std::cout << "  1. User: Only holds data" << std::endl;
    srp_good::User user("Bob", "bob@example.com", 25);

    std::cout << "  2. UserValidator: Only validates" << std::endl;
    auto result = srp_good::UserValidator::validate(user);
    std::cout << "     Validation result: " << result.message << std::endl;

    std::cout << "  3. EmailTemplates: Only formats emails" << std::endl;
    auto email = srp_good::EmailTemplates::welcomeEmail(user);
    std::cout << "     Email preview: " << email.substr(0, 30) << "..." << std::endl;

    std::cout << "  4. IUserRepository: Only handles persistence" << std::endl;
    auto repo = std::make_unique<srp_good::InMemoryUserRepository>();
    repo->save(user);
    std::cout << "     Users in repository: " << repo->count() << std::endl;

    std::cout << "  5. ILogger: Only logs messages" << std::endl;
    srp_good::ConsoleLogger logger;
    logger.log("User operations completed");
}

void demonstrate_testability() {
    std::cout << "\n=== Testability Comparison ===" << std::endl;

    std::cout << "\n  BAD approach - hard to test:" << std::endl;
    std::cout << "  - Need file system to test persistence" << std::endl;
    std::cout << "  - Can't test validation in isolation" << std::endl;
    std::cout << "  - Logging mixed with business logic" << std::endl;

    std::cout << "\n  GOOD approach - easy to test:" << std::endl;
    
    // Test validation in isolation
    auto nameResult = srp_good::UserValidator::validateName("");
    std::cout << "  - Validation test: empty name -> " << nameResult.message << std::endl;

    // Test with mock repository (in-memory)
    auto mockRepo = std::make_unique<srp_good::InMemoryUserRepository>();
    auto silentLogger = std::make_unique<srp_good::SilentLogger>();
    srp_good::UserService service(std::move(mockRepo), std::move(silentLogger));
    
    bool created = service.createUser("Test", "test@example.com", 20);
    std::cout << "  - Service test with mock: user created = " << (created ? "yes" : "no") << std::endl;
}

void demonstrate_srp_benefits() {
    std::cout << "\n=== Single Responsibility Principle Benefits ===" << std::endl;

    std::cout << "\n  1. TESTABILITY" << std::endl;
    std::cout << "     - Each class can be tested independently" << std::endl;
    std::cout << "     - Easy to mock dependencies" << std::endl;

    std::cout << "\n  2. MAINTAINABILITY" << std::endl;
    std::cout << "     - Changes are isolated to one class" << std::endl;
    std::cout << "     - Less risk of breaking unrelated features" << std::endl;

    std::cout << "\n  3. REUSABILITY" << std::endl;
    std::cout << "     - Validator can be used anywhere" << std::endl;
    std::cout << "     - Email templates reusable for any user type" << std::endl;

    std::cout << "\n  4. READABILITY" << std::endl;
    std::cout << "     - Each class has a clear purpose" << std::endl;
    std::cout << "     - Easier to understand and navigate" << std::endl;
}

} // end anonymous namespace

#include "SampleRegistry.hpp"

void SRPSample::run() {
    std::cout << "Running Single Responsibility Principle Sample..." << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << "SRP: A class should have only one reason to change." << std::endl;

    demonstrate_bad_srp();
    demonstrate_good_srp();
    demonstrate_testability();
    demonstrate_srp_benefits();

    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "1. Each class should have ONE responsibility" << std::endl;
    std::cout << "2. 'Responsibility' = 'reason to change'" << std::endl;
    std::cout << "3. Separate concerns: data, validation, persistence, formatting" << std::endl;
    std::cout << "4. Use interfaces to enable testing and flexibility" << std::endl;

    std::cout << "\nSingle Responsibility Principle demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(SRPSample, "Single Responsibility Principle", 18);
