#pragma once

#include <string>
#include <iostream>
#include <stdexcept>

namespace isp_bad {

// ============================================================================
// BAD EXAMPLE 1: Fat Interface (God Interface)
// ============================================================================
// Clients are forced to depend on methods they don't use.

class IWorker {
public:
    virtual ~IWorker() = default;
    virtual void work() = 0;
    virtual void eat() = 0;
    virtual void sleep() = 0;
    virtual void attendMeeting() = 0;
    virtual void writeReport() = 0;
    virtual void manageTeam() = 0;
};

// Regular employee - doesn't manage teams!
class Developer : public IWorker {
public:
    void work() override {
        std::cout << "    Developer writing code" << std::endl;
    }
    void eat() override {
        std::cout << "    Developer eating lunch" << std::endl;
    }
    void sleep() override {
        std::cout << "    Developer sleeping" << std::endl;
    }
    void attendMeeting() override {
        std::cout << "    Developer in meeting" << std::endl;
    }
    void writeReport() override {
        std::cout << "    Developer writing report" << std::endl;
    }
    // Forced to implement method they don't use!
    void manageTeam() override {
        throw std::logic_error("Developer doesn't manage teams!");
    }
};

// Robot worker - doesn't eat, sleep, or attend meetings!
class RobotWorker : public IWorker {
public:
    void work() override {
        std::cout << "    Robot working" << std::endl;
    }
    // Forced to implement methods that make no sense!
    void eat() override {
        throw std::logic_error("Robots don't eat!");
    }
    void sleep() override {
        throw std::logic_error("Robots don't sleep!");
    }
    void attendMeeting() override {
        throw std::logic_error("Robots don't attend meetings!");
    }
    void writeReport() override {
        throw std::logic_error("Robots don't write reports!");
    }
    void manageTeam() override {
        throw std::logic_error("Robots don't manage teams!");
    }
};

// ============================================================================
// BAD EXAMPLE 2: Multi-purpose Device Interface
// ============================================================================

class IMultiFunctionDevice {
public:
    virtual ~IMultiFunctionDevice() = default;
    virtual void print(const std::string& doc) = 0;
    virtual void scan(const std::string& doc) = 0;
    virtual void fax(const std::string& doc) = 0;
    virtual void copy(const std::string& doc) = 0;
    virtual void staple() = 0;
};

// Simple printer - forced to implement fax, scan, etc.!
class SimplePrinter : public IMultiFunctionDevice {
public:
    void print(const std::string& doc) override {
        std::cout << "    Printing: " << doc << std::endl;
    }
    void scan(const std::string&) override {
        throw std::logic_error("Simple printer can't scan!");
    }
    void fax(const std::string&) override {
        throw std::logic_error("Simple printer can't fax!");
    }
    void copy(const std::string&) override {
        throw std::logic_error("Simple printer can't copy!");
    }
    void staple() override {
        throw std::logic_error("Simple printer can't staple!");
    }
};

// ============================================================================
// BAD EXAMPLE 3: Repository with Everything
// ============================================================================

class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void create(const std::string& item) = 0;
    virtual std::string read(int id) = 0;
    virtual void update(int id, const std::string& item) = 0;
    virtual void remove(int id) = 0;  // "delete" is a keyword
    virtual void bulkInsert(const std::string& items) = 0;
    virtual void backup() = 0;
    virtual void restore() = 0;
    virtual void export_data() = 0;
    virtual void import_data() = 0;
};

// Read-only repository - forced to implement write operations!
class ReadOnlyCache : public IRepository {
public:
    std::string read(int id) override {
        return "cached_item_" + std::to_string(id);
    }
    // Forced to implement all these unusable methods!
    void create(const std::string&) override {
        throw std::logic_error("Read-only cache can't create!");
    }
    void update(int, const std::string&) override {
        throw std::logic_error("Read-only cache can't update!");
    }
    void remove(int) override {
        throw std::logic_error("Read-only cache can't delete!");
    }
    void bulkInsert(const std::string&) override {
        throw std::logic_error("Read-only cache can't bulk insert!");
    }
    void backup() override {
        throw std::logic_error("Read-only cache can't backup!");
    }
    void restore() override {
        throw std::logic_error("Read-only cache can't restore!");
    }
    void export_data() override {
        throw std::logic_error("Read-only cache can't export!");
    }
    void import_data() override {
        throw std::logic_error("Read-only cache can't import!");
    }
};

// Problems with fat interfaces:
// 1. Classes forced to implement irrelevant methods
// 2. Methods throw exceptions for "not supported"
// 3. Changes to interface affect unrelated classes
// 4. Difficult to understand what a class actually does
// 5. Testing requires mocking unused methods

} // namespace isp_bad
