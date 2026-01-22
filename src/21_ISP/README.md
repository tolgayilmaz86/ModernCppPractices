# Interface Segregation Principle (ISP)

## Overview

The Interface Segregation Principle states that **no client should be forced to depend on methods it does not use**.

> "Many client-specific interfaces are better than one general-purpose interface." — Robert C. Martin

## The Problem

When interfaces are too large ("fat interfaces"):
- **Forced implementations** — Classes must implement methods they don't need
- **"Not supported" exceptions** — Runtime errors instead of compile-time errors
- **Tight coupling** — Changes affect unrelated classes
- **Harder testing** — Must mock unused methods
- **Unclear contracts** — Hard to understand what a class actually does

## Bad Example ❌

```cpp
// FAT INTERFACE - too many methods!
class IWorker {
public:
    virtual void work() = 0;
    virtual void eat() = 0;
    virtual void sleep() = 0;
    virtual void attendMeeting() = 0;
    virtual void manageTeam() = 0;
};

class Developer : public IWorker {
    void work() override { /* ... */ }
    void eat() override { /* ... */ }
    void sleep() override { /* ... */ }
    void attendMeeting() override { /* ... */ }
    
    // Forced to implement - but developers don't manage teams!
    void manageTeam() override {
        throw std::logic_error("Not supported!");  // BAD!
    }
};

class RobotWorker : public IWorker {
    void work() override { /* ... */ }
    
    // Robots don't eat, sleep, or attend meetings!
    void eat() override { throw std::logic_error("Not supported!"); }
    void sleep() override { throw std::logic_error("Not supported!"); }
    void attendMeeting() override { throw std::logic_error("Not supported!"); }
    void manageTeam() override { throw std::logic_error("Not supported!"); }
};
```

**Problems:**
- `Developer` forced to implement `manageTeam()`
- `RobotWorker` forced to implement human-only methods
- Runtime exceptions instead of compile-time errors
- Unclear what each class actually supports

## Good Example ✅

```cpp
// SEGREGATED INTERFACES - each represents one capability
class IWorkable {
public:
    virtual void work() = 0;
};

class IFeedable {
public:
    virtual void eat() = 0;
};

class ISleepable {
public:
    virtual void sleep() = 0;
};

class ITeamManager {
public:
    virtual void manageTeam() = 0;
};

// Developer implements only relevant interfaces
class Developer : public IWorkable, public IFeedable, 
                  public ISleepable {
    void work() override { /* ... */ }
    void eat() override { /* ... */ }
    void sleep() override { /* ... */ }
    // No manageTeam() - doesn't need it!
};

// Manager adds team management
class Manager : public IWorkable, public IFeedable, 
                public ISleepable, public ITeamManager {
    void work() override { /* ... */ }
    void eat() override { /* ... */ }
    void sleep() override { /* ... */ }
    void manageTeam() override { /* ... */ }
};

// Robot only implements what makes sense
class RobotWorker : public IWorkable {
    void work() override { /* ... */ }
    // No eat(), sleep() - robots don't need them!
};
```

**Benefits:**
- Each class implements only what it needs
- Compile-time error if you call wrong method
- Clear what each class can do
- Easy to mock for testing

## Multi-Function Device Example

```cpp
// Bad: One fat interface
class IMultiFunctionDevice {
    virtual void print() = 0;
    virtual void scan() = 0;
    virtual void fax() = 0;
    virtual void copy() = 0;
    virtual void staple() = 0;
};

// Good: Segregated interfaces
class IPrinter { virtual void print() = 0; };
class IScanner { virtual std::string scan() = 0; };
class IFax { virtual void fax() = 0; };
class ICopier { virtual void copy() = 0; };

// Simple printer - only implements IPrinter
class SimplePrinter : public IPrinter {
    void print() override { /* ... */ }
};

// Multi-function device implements all
class MultiFunctionPrinter : public IPrinter, public IScanner, 
                             public IFax, public ICopier {
    void print() override { /* ... */ }
    std::string scan() override { /* ... */ }
    void fax() override { /* ... */ }
    void copy() override { /* ... */ }
};

// Functions depend on minimal interface
void printDocument(IPrinter& printer) {
    printer.print();  // Works with SimplePrinter AND MultiFunctionPrinter
}
```

## Repository Example

```cpp
// Segregated repository interfaces
class IReadable {
    virtual std::string read(int id) = 0;
};

class IWritable {
    virtual void create(const std::string& item) = 0;
    virtual void update(int id, const std::string& item) = 0;
    virtual void remove(int id) = 0;
};

class IBackupable {
    virtual void backup() = 0;
    virtual void restore() = 0;
};

// Read-only cache only implements reading
class ReadOnlyCache : public IReadable {
    std::string read(int id) override { return cached_data_[id]; }
};

// Full repository implements all
class FullRepository : public IReadable, public IWritable, public IBackupable {
    // Implements all methods
};

// Service depends on minimal interface
class ReportService {
    IReadable& dataSource_;  // Only needs reading!
public:
    ReportService(IReadable& source) : dataSource_(source) {}
    void generateReport() {
        auto data = dataSource_.read(1);
        // Generate report...
    }
};

// Works with either ReadOnlyCache or FullRepository!
```

## How to Apply ISP

| Approach | Description |
|----------|-------------|
| **Role interfaces** | One interface per role/capability |
| **Adapter pattern** | Create focused interface adapters |
| **Client-specific interfaces** | Design interfaces for client needs |
| **Composition** | Combine small interfaces as needed |

## Signs of ISP Violations

| Red Flag | Example |
|----------|---------|
| "Not supported" exceptions | `throw NotSupportedException()` |
| Empty method implementations | `void doSomething() { }` |
| Methods returning null/false always | `bool canFly() { return false; }` |
| `// TODO: Not applicable` comments | Comment instead of real implementation |
| Classes implementing > 7-10 methods | Interface likely too fat |

## Sample Output

```
Running Interface Segregation Principle Sample...

=== BAD: Fat Worker Interface ===
  Developer (implements all 6 methods):
    Developer writing code
    Developer eating lunch

  Developer calling manageTeam():
    [BAD] Exception: Developer doesn't manage teams!

  RobotWorker (forced to implement human methods):
    Robot working
    [BAD] Exception: Robots don't eat!

=== GOOD: Segregated Worker Interfaces ===
  Developer (IWorkable, IFeedable, IMeetingAttendee...):
    Developer writing code
    Developer eating lunch
    Developer in meeting

  Manager (adds ITeamManager):
    Manager reviewing work
    Manager managing team

  RobotWorker (only IWorkable):
    Robot working 24/7

=== GOOD: Segregated Device Interfaces ===
  SimplePrinter (IPrinter only):
    Printing: simple_doc.pdf

  MultiFunctionPrinter (IPrinter, IScanner, IFax, ICopier):
    MFP Printing: mfp_doc.pdf
    MFP Scanning...
    MFP Faxing: urgent.pdf
    MFP Copying 3 copies
```

## ISP vs Other Principles

| Principle | Relationship |
|-----------|--------------|
| **SRP** | ISP is SRP for interfaces |
| **LSP** | Segregated interfaces are easier to substitute correctly |
| **OCP** | Small interfaces are easier to extend |
| **DIP** | Small interfaces make better abstractions |

## Key Takeaways

1. **Split fat interfaces** — Many small interfaces > one large interface
2. **Role-based design** — One interface per capability/role
3. **Compile-time safety** — No "not supported" exceptions
4. **Minimal dependencies** — Clients depend only on what they use
5. **Easier testing** — Mock only the interface you need

## Further Reading

- [Interface Segregation Principle (Wikipedia)](https://en.wikipedia.org/wiki/Interface_segregation_principle)
- [Clean Code by Robert C. Martin](https://www.amazon.com/Clean-Code-Handbook-Software-Craftsmanship/dp/0132350882)
