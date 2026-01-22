# Dependency Inversion Principle (DIP)

## Overview

The Dependency Inversion Principle has two parts:
1. **High-level modules should not depend on low-level modules. Both should depend on abstractions.**
2. **Abstractions should not depend on details. Details should depend on abstractions.**

> "Depend on abstractions, not concretions." — Robert C. Martin

## The Problem

When high-level modules depend directly on low-level modules:
- **Tight coupling** — Can't change implementations without modifying callers
- **Hard to test** — Can't mock dependencies for unit tests
- **Rigid design** — Changes cascade through the codebase
- **No flexibility** — Stuck with one implementation forever

## Bad Example ❌

```cpp
// LOW-LEVEL: Concrete implementation
class MySQLDatabase {
public:
    void save(const std::string& data) { /* MySQL-specific code */ }
};

class EmailService {
public:
    void send(const std::string& to, const std::string& msg) { /* SMTP code */ }
};

// HIGH-LEVEL: Depends directly on concrete classes!
class OrderProcessor {
private:
    MySQLDatabase database_;      // Direct dependency!
    EmailService emailService_;   // Direct dependency!

public:
    void processOrder(const std::string& order) {
        database_.save(order);     // Stuck with MySQL!
        emailService_.send("...", "Order confirmed");  // Stuck with Email!
    }
};
```

**Problems:**
- Can't use PostgreSQL without modifying `OrderProcessor`
- Can't use SMS instead of email
- Can't unit test without a real database
- Every change requires recompilation

## Good Example ✅

```cpp
// ABSTRACTIONS: Interfaces
class IDatabase {
public:
    virtual void save(const std::string& data) = 0;
    virtual ~IDatabase() = default;
};

class INotificationService {
public:
    virtual void send(const std::string& to, const std::string& msg) = 0;
    virtual ~INotificationService() = default;
};

// LOW-LEVEL: Implementations depend on abstractions
class MySQLDatabase : public IDatabase {
    void save(const std::string& data) override { /* MySQL code */ }
};

class PostgreSQLDatabase : public IDatabase {
    void save(const std::string& data) override { /* PostgreSQL code */ }
};

class EmailService : public INotificationService {
    void send(const std::string& to, const std::string& msg) override { /* ... */ }
};

class SMSService : public INotificationService {
    void send(const std::string& to, const std::string& msg) override { /* ... */ }
};

// HIGH-LEVEL: Depends on abstractions via dependency injection
class OrderProcessor {
private:
    IDatabase& database_;
    INotificationService& notifier_;

public:
    // Dependencies INJECTED through constructor
    OrderProcessor(IDatabase& db, INotificationService& notifier)
        : database_(db), notifier_(notifier) {}

    void processOrder(const std::string& order) {
        database_.save(order);           // Any IDatabase works!
        notifier_.send("...", "Order confirmed");  // Any notifier works!
    }
};

// Usage - easily swap implementations!
MySQLDatabase mysql;
EmailService email;
OrderProcessor prod(mysql, email);  // Production

PostgreSQLDatabase postgres;
SMSService sms;
OrderProcessor alt(postgres, sms);  // Different config - no code changes!

InMemoryDatabase memDb;
MockNotifier mock;
OrderProcessor test(memDb, mock);   // Testing - with mocks!
```

## Dependency Injection Types

### Constructor Injection (Recommended)
```cpp
class Service {
    IDependency& dep_;
public:
    explicit Service(IDependency& dep) : dep_(dep) {}
};
```

### Setter Injection
```cpp
class Service {
    IDependency* dep_ = nullptr;
public:
    void setDependency(IDependency& dep) { dep_ = &dep; }
};
```

### Interface Injection
```cpp
class IDependencyInjector {
public:
    virtual void inject(IDependency& dep) = 0;
};

class Service : public IDependencyInjector {
    IDependency* dep_;
public:
    void inject(IDependency& dep) override { dep_ = &dep; }
};
```

## The Inversion

| Traditional (Bad) | Inverted (Good) |
|-------------------|-----------------|
| High-level → Low-level | High-level → Abstraction ← Low-level |
| `OrderProcessor` → `MySQLDatabase` | `OrderProcessor` → `IDatabase` ← `MySQLDatabase` |
| Policy knows implementation | Policy knows only interface |

**Before:** High-level controls low-level  
**After:** Both depend on shared abstraction (arrows point toward abstraction)

## Testing Benefits

```cpp
// Mock for testing
class MockDatabase : public IDatabase {
    std::vector<std::string> saved_;
public:
    void save(const std::string& data) override {
        saved_.push_back(data);  // Just record - no real DB!
    }
    bool wasSaved(const std::string& data) {
        return std::find(saved_.begin(), saved_.end(), data) != saved_.end();
    }
};

class MockNotifier : public INotificationService {
    int sendCount_ = 0;
public:
    void send(const std::string&, const std::string&) override {
        sendCount_++;
    }
    int getSendCount() { return sendCount_; }
};

// Unit test - no real database or email server needed!
void testOrderProcessor() {
    MockDatabase mockDb;
    MockNotifier mockNotifier;
    OrderProcessor processor(mockDb, mockNotifier);
    
    processor.processOrder("ORD-123");
    
    assert(mockDb.wasSaved("ORD-123"));
    assert(mockNotifier.getSendCount() == 1);
}
```

## Sample Output

```
Running Dependency Inversion Principle Sample...

=== BAD: Direct Dependencies on Concrete Classes ===
  Problems with tight coupling:
  - OrderProcessor is hardcoded to MySQL, Email, PDF
  - Can't switch to PostgreSQL without modifying code

  OrderProcessor (BAD - tight coupling):
    Saving to MySQL: ORD-12345
    Sending email to customer@example.com: Order confirmed
    Generating PDF: Invoice for ORD-12345

=== GOOD: Dependency Injection with Abstractions ===

  Production Setup (MySQL + Email + PDF):
  OrderProcessor (GOOD - depends on abstractions):
    Saving to MySQL: ORD-PROD-001
    Sending email to customer@example.com: Order confirmed
    Generating PDF: Invoice for ORD-PROD-001

  Alternative Setup (PostgreSQL + SMS + Word):
  OrderProcessor (GOOD - depends on abstractions):
    Saving to PostgreSQL: ORD-ALT-002
    Sending SMS to customer@example.com: Order confirmed
    Generating Word doc: Invoice for ORD-ALT-002

  Test Setup (InMemory + Push + PDF):
  OrderProcessor (GOOD - depends on abstractions):
    Saving to memory: ORD-TEST-003
    Sending push to customer@example.com: Order confirmed
    Generating PDF: Invoice for ORD-TEST-003
```

## DIP vs DI vs IoC

| Term | Meaning |
|------|---------|
| **DIP** (Dependency Inversion Principle) | High-level shouldn't depend on low-level |
| **DI** (Dependency Injection) | Technique to achieve DIP by injecting dependencies |
| **IoC** (Inversion of Control) | Broader concept - framework calls your code |

DIP is the **principle**, DI is a **technique** to implement it, IoC is the **pattern**.

## DIP and SOLID

| Principle | How DIP Helps |
|-----------|---------------|
| **SRP** | Each class has one responsibility |
| **OCP** | Open for extension via new implementations |
| **LSP** | Implementations must be substitutable |
| **ISP** | Small interfaces make better abstractions |
| **DIP** | Ties everything together with abstractions |

## Key Takeaways

1. **Depend on abstractions** — Interfaces over concrete classes
2. **Inject dependencies** — Don't create them inside the class
3. **High-level defines abstractions** — Low-level implements them
4. **Easy testing** — Mock implementations for unit tests
5. **Flexibility** — Swap implementations without code changes

## Further Reading

- [Dependency Inversion Principle (Wikipedia)](https://en.wikipedia.org/wiki/Dependency_inversion_principle)
- [Dependency Injection (Martin Fowler)](https://martinfowler.com/articles/injection.html)
- [Clean Architecture by Robert C. Martin](https://www.amazon.com/Clean-Architecture-Craftsmans-Software-Structure/dp/0134494164)
