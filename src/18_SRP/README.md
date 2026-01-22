# Single Responsibility Principle (SRP)

## Overview

The Single Responsibility Principle states that **a class should have only one reason to change**. This means each class should have exactly one job or responsibility.

> "Gather together the things that change for the same reasons. Separate those things that change for different reasons." — Robert C. Martin

## The Problem

When a class handles multiple responsibilities, it becomes:
- **Hard to test** — Can't test one responsibility without the others
- **Hard to maintain** — Changes to one feature might break another
- **Hard to reuse** — Can't use validation logic without creating a full User
- **Tightly coupled** — Everything depends on everything

## Bad Example ❌

```cpp
// This class has 5 responsibilities!
class User {
public:
    // 1. Data management
    std::string getName() const;
    void setName(const std::string& name);
    
    // 2. Validation
    bool validateEmail(const std::string& email);
    bool validateName(const std::string& name);
    
    // 3. Persistence
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
    
    // 4. Email formatting
    std::string formatWelcomeEmail();
    std::string formatPasswordResetEmail();
    
    // 5. Logging
    void log(const std::string& message);
};
```

**Problems:**
- Want to change how logging works? Modify User class.
- Want to change email format? Modify User class.
- Want to add database persistence? Modify User class.
- Everything touches the User class!

## Good Example ✅

```cpp
// 1. Data only
class User {
    std::string name_, email_;
    int age_;
public:
    std::string getName() const;
    void setName(const std::string& name);
    // ... only data access
};

// 2. Validation only
class UserValidator {
public:
    static ValidationResult validateName(const std::string& name);
    static ValidationResult validateEmail(const std::string& email);
    static ValidationResult validate(const User& user);
};

// 3. Persistence only (with interface for testing)
class IUserRepository {
public:
    virtual bool save(const User& user) = 0;
    virtual std::unique_ptr<User> load(const std::string& id) = 0;
};

class FileUserRepository : public IUserRepository { /* file-based */ };
class InMemoryUserRepository : public IUserRepository { /* for testing */ };

// 4. Email formatting only
class EmailTemplates {
public:
    static std::string welcomeEmail(const User& user);
    static std::string passwordResetEmail(const User& user);
};

// 5. Logging only
class ILogger {
public:
    virtual void log(const std::string& message) = 0;
};
```

## How to Identify SRP Violations

Ask yourself:
1. **"What does this class do?"** — If you use "AND", it might violate SRP
2. **"Who would request changes?"** — Different stakeholders = different responsibilities
3. **"Can I test this in isolation?"** — If not, responsibilities are coupled

| Red Flag | Example |
|----------|---------|
| Class name includes "And" | `UserAndEmailManager` |
| Methods that don't use class state | `user.formatDate()` |
| Multiple unrelated imports | DB, HTTP, File, Email in one class |
| Large classes (>200 lines) | Often doing too much |

## Benefits of SRP

| Benefit | Explanation |
|---------|-------------|
| **Testability** | Test each responsibility in isolation |
| **Maintainability** | Changes are localized |
| **Reusability** | Use validator anywhere, not just with User |
| **Readability** | Each class has a clear, focused purpose |
| **Flexibility** | Easy to swap implementations (file → database) |

## Real-World Examples

| Violation | Better Design |
|-----------|---------------|
| `Order` class that calculates totals AND sends emails | `Order` + `OrderCalculator` + `OrderNotifier` |
| `Report` class that generates AND prints AND exports | `ReportGenerator` + `ReportPrinter` + `ReportExporter` |
| `Controller` that handles HTTP AND business logic | `Controller` + `Service` + `Repository` |

## Common Misconceptions

### "SRP means one method per class"
**Wrong!** A class can have many methods, as long as they serve the same responsibility.

### "SRP means classes should be tiny"
**Wrong!** A `UserValidator` with 10 validation methods is fine — they're all validation.

### "SRP is about code organization"
**Partially true.** It's really about **change management** — who asks for changes and why.

## Sample Output

```
Running Single Responsibility Principle Sample...

=== BAD: Class with Multiple Responsibilities ===
The User class handles: data, validation, persistence, email, logging

  [BAD] User created: Alice
  [BAD] Name changed from Alice to Alice Smith
  [BAD] Validation failed: invalid email format

=== GOOD: Single Responsibility per Class ===
Each class has ONE job:

  1. User: Only holds data
  2. UserValidator: Only validates
     Validation result: User is valid
  3. EmailTemplates: Only formats emails
  4. IUserRepository: Only handles persistence
  5. ILogger: Only logs messages
     [GOOD][LOG] User operations completed

=== Key Takeaways ===
1. Each class should have ONE responsibility
2. 'Responsibility' = 'reason to change'
3. Separate concerns: data, validation, persistence, formatting
4. Use interfaces to enable testing and flexibility
```

## Further Reading

- [Clean Code by Robert C. Martin](https://www.amazon.com/Clean-Code-Handbook-Software-Craftsmanship/dp/0132350882)
- [SOLID Principles Explained](https://www.digitalocean.com/community/conceptual_articles/s-o-l-i-d-the-first-five-principles-of-object-oriented-design)
