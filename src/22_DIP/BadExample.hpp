#pragma once

#include <string>
#include <iostream>
#include <vector>

namespace dip_bad {

// ============================================================================
// BAD EXAMPLE 1: High-level module depends on low-level modules
// ============================================================================
// The OrderProcessor (high-level) directly depends on concrete implementations
// (low-level modules). This makes it impossible to change without modifying.

class MySQLDatabase {
public:
    void save(const std::string& data) {
        std::cout << "    Saving to MySQL: " << data << std::endl;
    }
    std::string load(int id) {
        return "MySQL_data_" + std::to_string(id);
    }
};

class EmailService {
public:
    void sendEmail(const std::string& to, const std::string& message) {
        std::cout << "    Sending email to " << to << ": " << message << std::endl;
    }
};

class PDFGenerator {
public:
    std::string generate(const std::string& content) {
        std::cout << "    Generating PDF: " << content << std::endl;
        return "pdf_" + content;
    }
};

// HIGH-LEVEL MODULE - but it depends on concrete classes!
class OrderProcessor {
private:
    MySQLDatabase database_;      // Direct dependency on MySQL!
    EmailService emailService_;   // Direct dependency on EmailService!
    PDFGenerator pdfGenerator_;   // Direct dependency on PDFGenerator!

public:
    void processOrder(const std::string& order) {
        std::cout << "\n  OrderProcessor (BAD - tight coupling):" << std::endl;
        
        // Can't change database without modifying this class!
        database_.save(order);
        
        // Can't change email provider without modifying this class!
        emailService_.sendEmail("customer@example.com", "Order confirmed");
        
        // Can't change document format without modifying this class!
        pdfGenerator_.generate("Invoice for " + order);
    }
};

// Problems:
// 1. Can't use PostgreSQL without modifying OrderProcessor
// 2. Can't use SMS instead of email without modifying OrderProcessor
// 3. Can't generate Word documents without modifying OrderProcessor
// 4. Can't unit test OrderProcessor in isolation
// 5. Every change requires recompilation of OrderProcessor

// ============================================================================
// BAD EXAMPLE 2: Notification System with Direct Dependencies
// ============================================================================

class ConsoleLogger {
public:
    void log(const std::string& message) {
        std::cout << "    [CONSOLE] " << message << std::endl;
    }
};

class UserRepository {
public:
    std::string getUserEmail(int userId) {
        return "user" + std::to_string(userId) + "@example.com";
    }
};

// Tightly coupled notification system
class NotificationSystem {
private:
    ConsoleLogger logger_;           // Direct dependency!
    UserRepository userRepo_;        // Direct dependency!
    EmailService emailService_;      // Direct dependency!

public:
    void notifyUser(int userId, const std::string& message) {
        std::cout << "\n  NotificationSystem (BAD - tight coupling):" << std::endl;
        
        logger_.log("Notifying user " + std::to_string(userId));
        
        std::string email = userRepo_.getUserEmail(userId);
        emailService_.sendEmail(email, message);
        
        logger_.log("Notification sent");
    }
};

// ============================================================================
// BAD EXAMPLE 3: Payment Processing with Hardcoded Dependency
// ============================================================================

class StripePayment {
public:
    bool charge(double amount) {
        std::cout << "    Charging $" << amount << " via Stripe" << std::endl;
        return true;
    }
};

class PaymentService {
private:
    StripePayment stripe_;  // Can't change payment provider!

public:
    bool processPayment(double amount) {
        std::cout << "\n  PaymentService (BAD - hardcoded Stripe):" << std::endl;
        return stripe_.charge(amount);  // Stuck with Stripe forever!
    }
};

// Problems with these designs:
// 1. High-level modules depend on low-level implementations
// 2. No way to substitute implementations (for testing or flexibility)
// 3. Changes cascade through the codebase
// 4. Violates DIP: both should depend on abstractions

} // namespace dip_bad
