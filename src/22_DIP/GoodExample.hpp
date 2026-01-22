#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>

namespace dip_good {

// ============================================================================
// GOOD EXAMPLE 1: Abstractions for Low-Level Modules
// ============================================================================
// Both high-level and low-level modules depend on abstractions.

// ABSTRACTION: Database interface
class IDatabase {
public:
    virtual ~IDatabase() = default;
    virtual void save(const std::string& data) = 0;
    virtual std::string load(int id) = 0;
};

// ABSTRACTION: Notification interface
class INotificationService {
public:
    virtual ~INotificationService() = default;
    virtual void send(const std::string& to, const std::string& message) = 0;
};

// ABSTRACTION: Document generator interface
class IDocumentGenerator {
public:
    virtual ~IDocumentGenerator() = default;
    virtual std::string generate(const std::string& content) = 0;
};

// LOW-LEVEL: MySQL implementation
class MySQLDatabase : public IDatabase {
public:
    void save(const std::string& data) override {
        std::cout << "    Saving to MySQL: " << data << std::endl;
    }
    std::string load(int id) override {
        return "MySQL_data_" + std::to_string(id);
    }
};

// LOW-LEVEL: PostgreSQL implementation
class PostgreSQLDatabase : public IDatabase {
public:
    void save(const std::string& data) override {
        std::cout << "    Saving to PostgreSQL: " << data << std::endl;
    }
    std::string load(int id) override {
        return "PostgreSQL_data_" + std::to_string(id);
    }
};

// LOW-LEVEL: In-memory database (great for testing!)
class InMemoryDatabase : public IDatabase {
private:
    std::vector<std::string> data_;
public:
    void save(const std::string& data) override {
        data_.push_back(data);
        std::cout << "    Saving to memory: " << data << std::endl;
    }
    std::string load(int id) override {
        if (id >= 0 && id < static_cast<int>(data_.size())) {
            return data_[static_cast<size_t>(id)];
        }
        return "";
    }
};

// LOW-LEVEL: Email service
class EmailService : public INotificationService {
public:
    void send(const std::string& to, const std::string& message) override {
        std::cout << "    Sending email to " << to << ": " << message << std::endl;
    }
};

// LOW-LEVEL: SMS service
class SMSService : public INotificationService {
public:
    void send(const std::string& to, const std::string& message) override {
        std::cout << "    Sending SMS to " << to << ": " << message << std::endl;
    }
};

// LOW-LEVEL: Push notification service
class PushNotificationService : public INotificationService {
public:
    void send(const std::string& to, const std::string& message) override {
        std::cout << "    Sending push to " << to << ": " << message << std::endl;
    }
};

// LOW-LEVEL: PDF generator
class PDFGenerator : public IDocumentGenerator {
public:
    std::string generate(const std::string& content) override {
        std::cout << "    Generating PDF: " << content << std::endl;
        return "pdf_" + content;
    }
};

// LOW-LEVEL: Word generator
class WordGenerator : public IDocumentGenerator {
public:
    std::string generate(const std::string& content) override {
        std::cout << "    Generating Word doc: " << content << std::endl;
        return "docx_" + content;
    }
};

// HIGH-LEVEL: Depends on abstractions via dependency injection
class OrderProcessor {
private:
    IDatabase& database_;
    INotificationService& notifier_;
    IDocumentGenerator& docGenerator_;

public:
    // Dependencies are INJECTED through constructor
    OrderProcessor(IDatabase& db, INotificationService& notifier, IDocumentGenerator& docGen)
        : database_(db), notifier_(notifier), docGenerator_(docGen) {}

    void processOrder(const std::string& order) {
        std::cout << "\n  OrderProcessor (GOOD - depends on abstractions):" << std::endl;
        
        // Uses abstraction - doesn't know which database
        database_.save(order);
        
        // Uses abstraction - doesn't know which notification method
        notifier_.send("customer@example.com", "Order confirmed");
        
        // Uses abstraction - doesn't know which document format
        docGenerator_.generate("Invoice for " + order);
    }
};

// ============================================================================
// GOOD EXAMPLE 2: Logger Abstraction
// ============================================================================

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

class ConsoleLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "    [CONSOLE] " << message << std::endl;
    }
};

class FileLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "    [FILE] " << message << std::endl;
    }
};

// ============================================================================
// GOOD EXAMPLE 3: User Repository Abstraction
// ============================================================================

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::string getUserEmail(int userId) = 0;
};

class DatabaseUserRepository : public IUserRepository {
public:
    std::string getUserEmail(int userId) override {
        return "user" + std::to_string(userId) + "@example.com";
    }
};

class MockUserRepository : public IUserRepository {
public:
    std::string getUserEmail(int) override {
        return "mock@test.com";  // For testing!
    }
};

// High-level notification system with injected dependencies
class NotificationSystem {
private:
    ILogger& logger_;
    IUserRepository& userRepo_;
    INotificationService& notifier_;

public:
    NotificationSystem(ILogger& logger, IUserRepository& userRepo, INotificationService& notifier)
        : logger_(logger), userRepo_(userRepo), notifier_(notifier) {}

    void notifyUser(int userId, const std::string& message) {
        std::cout << "\n  NotificationSystem (GOOD - injected dependencies):" << std::endl;
        
        logger_.log("Notifying user " + std::to_string(userId));
        
        std::string email = userRepo_.getUserEmail(userId);
        notifier_.send(email, message);
        
        logger_.log("Notification sent");
    }
};

// ============================================================================
// GOOD EXAMPLE 4: Payment Processing with Abstraction
// ============================================================================

class IPaymentGateway {
public:
    virtual ~IPaymentGateway() = default;
    virtual bool charge(double amount) = 0;
    virtual std::string name() const = 0;
};

class StripeGateway : public IPaymentGateway {
public:
    bool charge(double amount) override {
        std::cout << "    Charging $" << amount << " via Stripe" << std::endl;
        return true;
    }
    std::string name() const override { return "Stripe"; }
};

class PayPalGateway : public IPaymentGateway {
public:
    bool charge(double amount) override {
        std::cout << "    Charging $" << amount << " via PayPal" << std::endl;
        return true;
    }
    std::string name() const override { return "PayPal"; }
};

class MockGateway : public IPaymentGateway {
public:
    bool charge(double amount) override {
        std::cout << "    [MOCK] Would charge $" << amount << std::endl;
        return true;  // Always succeeds for testing
    }
    std::string name() const override { return "Mock"; }
};

class PaymentService {
private:
    IPaymentGateway& gateway_;

public:
    explicit PaymentService(IPaymentGateway& gateway) : gateway_(gateway) {}

    bool processPayment(double amount) {
        std::cout << "\n  PaymentService (GOOD - using " << gateway_.name() << "):" << std::endl;
        return gateway_.charge(amount);
    }
};

// Benefits of this design:
// 1. High-level modules depend on abstractions (interfaces)
// 2. Low-level modules also depend on abstractions (implement interfaces)
// 3. Easy to swap implementations (MySQL → PostgreSQL, Email → SMS)
// 4. Easy to test with mocks
// 5. No changes to high-level modules when adding new implementations

} // namespace dip_good
