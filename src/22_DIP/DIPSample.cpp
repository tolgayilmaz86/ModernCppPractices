#include "DIPSample.hpp"
#include "BadExample.hpp"
#include "GoodExample.hpp"
#include <iostream>
#include <memory>

void demonstrate_bad_dip() {
    std::cout << "\n=== BAD: Direct Dependencies on Concrete Classes ===" << std::endl;
    std::cout << "High-level modules depend on low-level implementations\n" << std::endl;

    std::cout << "  Problems with tight coupling:" << std::endl;
    std::cout << "  - OrderProcessor is hardcoded to MySQL, Email, PDF" << std::endl;
    std::cout << "  - Can't switch to PostgreSQL without modifying code" << std::endl;
    std::cout << "  - Can't use SMS instead of Email" << std::endl;
    std::cout << "  - Can't unit test in isolation" << std::endl;

    dip_bad::OrderProcessor processor;
    processor.processOrder("ORD-12345");

    dip_bad::NotificationSystem notifier;
    notifier.notifyUser(42, "Your order has shipped!");

    dip_bad::PaymentService payment;
    payment.processPayment(99.99);
}

void demonstrate_good_order_processor() {
    std::cout << "\n=== GOOD: Dependency Injection with Abstractions ===" << std::endl;
    std::cout << "High-level modules depend on interfaces, not implementations\n" << std::endl;

    // Production configuration
    std::cout << "  Production Setup (MySQL + Email + PDF):" << std::endl;
    dip_good::MySQLDatabase mysql;
    dip_good::EmailService email;
    dip_good::PDFGenerator pdf;
    dip_good::OrderProcessor prodProcessor(mysql, email, pdf);
    prodProcessor.processOrder("ORD-PROD-001");

    // Different configuration - no code changes!
    std::cout << "\n  Alternative Setup (PostgreSQL + SMS + Word):" << std::endl;
    dip_good::PostgreSQLDatabase postgres;
    dip_good::SMSService sms;
    dip_good::WordGenerator word;
    dip_good::OrderProcessor altProcessor(postgres, sms, word);
    altProcessor.processOrder("ORD-ALT-002");

    // Test configuration with mocks
    std::cout << "\n  Test Setup (InMemory + Push + PDF):" << std::endl;
    dip_good::InMemoryDatabase memDb;
    dip_good::PushNotificationService push;
    dip_good::OrderProcessor testProcessor(memDb, push, pdf);
    testProcessor.processOrder("ORD-TEST-003");
}

void demonstrate_good_notification_system() {
    std::cout << "\n=== GOOD: Notification System with DI ===" << std::endl;
    std::cout << "Easy to swap logger, user repo, and notification method\n" << std::endl;

    // Production setup
    std::cout << "  Production (ConsoleLogger + DatabaseUserRepo + Email):" << std::endl;
    dip_good::ConsoleLogger consoleLog;
    dip_good::DatabaseUserRepository dbUserRepo;
    dip_good::EmailService email;
    dip_good::NotificationSystem prodNotifier(consoleLog, dbUserRepo, email);
    prodNotifier.notifyUser(1, "Welcome to our service!");

    // Test setup with mocks
    std::cout << "\n  Testing (FileLogger + MockUserRepo + SMS):" << std::endl;
    dip_good::FileLogger fileLog;
    dip_good::MockUserRepository mockUserRepo;
    dip_good::SMSService sms;
    dip_good::NotificationSystem testNotifier(fileLog, mockUserRepo, sms);
    testNotifier.notifyUser(999, "Test notification");
}

void demonstrate_good_payment_service() {
    std::cout << "\n=== GOOD: Payment Service with Gateway Abstraction ===" << std::endl;
    std::cout << "Switch payment providers without changing PaymentService\n" << std::endl;

    dip_good::StripeGateway stripe;
    dip_good::PayPalGateway paypal;
    dip_good::MockGateway mock;

    dip_good::PaymentService stripeService(stripe);
    stripeService.processPayment(100.00);

    dip_good::PaymentService paypalService(paypal);
    paypalService.processPayment(75.50);

    std::cout << "\n  Testing with mock gateway:" << std::endl;
    dip_good::PaymentService testService(mock);
    testService.processPayment(999.99);
}

void demonstrate_dip_principles() {
    std::cout << "\n=== Dependency Inversion Principles ===" << std::endl;

    std::cout << "\n  1. HIGH-LEVEL MODULES should not depend on LOW-LEVEL MODULES" << std::endl;
    std::cout << "     Both should depend on ABSTRACTIONS" << std::endl;

    std::cout << "\n  2. ABSTRACTIONS should not depend on DETAILS" << std::endl;
    std::cout << "     DETAILS should depend on ABSTRACTIONS" << std::endl;

    std::cout << "\n  DEPENDENCY INJECTION TYPES:" << std::endl;
    std::cout << "     - Constructor Injection (recommended)" << std::endl;
    std::cout << "     - Setter Injection" << std::endl;
    std::cout << "     - Interface Injection" << std::endl;

    std::cout << "\n  BENEFITS:" << std::endl;
    std::cout << "     - Loose coupling" << std::endl;
    std::cout << "     - Easy testing with mocks" << std::endl;
    std::cout << "     - Flexibility to swap implementations" << std::endl;
    std::cout << "     - Adherence to OCP (open for extension)" << std::endl;
}

#include "SampleRegistry.hpp"

void DIPSample::run() {
    std::cout << "Running Dependency Inversion Principle Sample..." << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << "DIP: High-level modules should not depend on low-level modules." << std::endl;
    std::cout << "     Both should depend on abstractions." << std::endl;

    demonstrate_bad_dip();
    demonstrate_good_order_processor();
    demonstrate_good_notification_system();
    demonstrate_good_payment_service();
    demonstrate_dip_principles();

    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "1. Depend on abstractions (interfaces), not concrete classes" << std::endl;
    std::cout << "2. Inject dependencies through constructor" << std::endl;
    std::cout << "3. High-level policy shouldn't know low-level details" << std::endl;
    std::cout << "4. Makes testing easy with mock implementations" << std::endl;

    std::cout << "\nDependency Inversion Principle demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(DIPSample, "Dependency Inversion Principle", 22);
