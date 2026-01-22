#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>

namespace isp_good {

// ============================================================================
// GOOD EXAMPLE 1: Segregated Worker Interfaces
// ============================================================================
// Each interface represents a single capability.

class IWorkable {
public:
    virtual ~IWorkable() = default;
    virtual void work() = 0;
};

class IFeedable {
public:
    virtual ~IFeedable() = default;
    virtual void eat() = 0;
};

class ISleepable {
public:
    virtual ~ISleepable() = default;
    virtual void sleep() = 0;
};

class IMeetingAttendee {
public:
    virtual ~IMeetingAttendee() = default;
    virtual void attendMeeting() = 0;
};

class IReportWriter {
public:
    virtual ~IReportWriter() = default;
    virtual void writeReport() = 0;
};

class ITeamManager {
public:
    virtual ~ITeamManager() = default;
    virtual void manageTeam() = 0;
};

// Developer only implements what it needs
class Developer : public IWorkable, public IFeedable, 
                  public ISleepable, public IMeetingAttendee, 
                  public IReportWriter {
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
    // No manageTeam() - Developer doesn't need it!
};

// Manager adds team management
class Manager : public IWorkable, public IFeedable, 
                public ISleepable, public IMeetingAttendee, 
                public IReportWriter, public ITeamManager {
public:
    void work() override {
        std::cout << "    Manager reviewing work" << std::endl;
    }
    void eat() override {
        std::cout << "    Manager having lunch meeting" << std::endl;
    }
    void sleep() override {
        std::cout << "    Manager sleeping" << std::endl;
    }
    void attendMeeting() override {
        std::cout << "    Manager leading meeting" << std::endl;
    }
    void writeReport() override {
        std::cout << "    Manager writing status report" << std::endl;
    }
    void manageTeam() override {
        std::cout << "    Manager managing team" << std::endl;
    }
};

// Robot only implements what makes sense
class RobotWorker : public IWorkable {
public:
    void work() override {
        std::cout << "    Robot working 24/7" << std::endl;
    }
    // No eat(), sleep(), etc. - makes no sense for a robot!
};

// ============================================================================
// GOOD EXAMPLE 2: Segregated Device Interfaces
// ============================================================================

class IPrinter {
public:
    virtual ~IPrinter() = default;
    virtual void print(const std::string& doc) = 0;
};

class IScanner {
public:
    virtual ~IScanner() = default;
    virtual std::string scan() = 0;
};

class IFax {
public:
    virtual ~IFax() = default;
    virtual void fax(const std::string& doc) = 0;
};

class ICopier {
public:
    virtual ~ICopier() = default;
    virtual void copy(int copies) = 0;
};

// Simple printer - only implements printing
class SimplePrinter : public IPrinter {
public:
    void print(const std::string& doc) override {
        std::cout << "    Printing: " << doc << std::endl;
    }
    // No scan, fax, copy methods needed!
};

// Scanner - only implements scanning
class SimpleScanner : public IScanner {
public:
    std::string scan() override {
        std::cout << "    Scanning document..." << std::endl;
        return "scanned_content";
    }
};

// Multi-function device implements all relevant interfaces
class MultiFunctionPrinter : public IPrinter, public IScanner, 
                             public IFax, public ICopier {
public:
    void print(const std::string& doc) override {
        std::cout << "    MFP Printing: " << doc << std::endl;
    }
    std::string scan() override {
        std::cout << "    MFP Scanning..." << std::endl;
        return "scanned_content";
    }
    void fax(const std::string& doc) override {
        std::cout << "    MFP Faxing: " << doc << std::endl;
    }
    void copy(int copies) override {
        std::cout << "    MFP Copying " << copies << " copies" << std::endl;
    }
};

// ============================================================================
// GOOD EXAMPLE 3: Segregated Repository Interfaces
// ============================================================================

class IReadable {
public:
    virtual ~IReadable() = default;
    virtual std::string read(int id) = 0;
};

class IWritable {
public:
    virtual ~IWritable() = default;
    virtual void create(const std::string& item) = 0;
    virtual void update(int id, const std::string& item) = 0;
    virtual void remove(int id) = 0;
};

class IBulkOperations {
public:
    virtual ~IBulkOperations() = default;
    virtual void bulkInsert(const std::vector<std::string>& items) = 0;
};

class IBackupable {
public:
    virtual ~IBackupable() = default;
    virtual void backup() = 0;
    virtual void restore() = 0;
};

class IExportable {
public:
    virtual ~IExportable() = default;
    virtual std::string exportData() = 0;
    virtual void importData(const std::string& data) = 0;
};

// Read-only cache - only implements reading
class ReadOnlyCache : public IReadable {
public:
    std::string read(int id) override {
        return "cached_item_" + std::to_string(id);
    }
    // No write methods needed!
};

// Full repository implements all needed interfaces
class FullRepository : public IReadable, public IWritable, 
                       public IBulkOperations, public IBackupable {
private:
    std::vector<std::string> data_;
public:
    std::string read(int id) override {
        if (id >= 0 && id < static_cast<int>(data_.size())) {
            return data_[static_cast<size_t>(id)];
        }
        return "";
    }
    void create(const std::string& item) override {
        data_.push_back(item);
        std::cout << "    Created: " << item << std::endl;
    }
    void update(int id, const std::string& item) override {
        if (id >= 0 && id < static_cast<int>(data_.size())) {
            data_[static_cast<size_t>(id)] = item;
            std::cout << "    Updated id " << id << ": " << item << std::endl;
        }
    }
    void remove(int id) override {
        if (id >= 0 && id < static_cast<int>(data_.size())) {
            data_.erase(data_.begin() + id);
            std::cout << "    Removed id " << id << std::endl;
        }
    }
    void bulkInsert(const std::vector<std::string>& items) override {
        for (const auto& item : items) {
            data_.push_back(item);
        }
        std::cout << "    Bulk inserted " << items.size() << " items" << std::endl;
    }
    void backup() override {
        std::cout << "    Backed up " << data_.size() << " items" << std::endl;
    }
    void restore() override {
        std::cout << "    Restored data" << std::endl;
    }
};

// Functions can depend on minimal interfaces
void printDocument(IPrinter& printer, const std::string& doc) {
    printer.print(doc);
}

void readFromSource(IReadable& source, int id) {
    std::cout << "    Read: " << source.read(id) << std::endl;
}

// Benefits of this design:
// 1. Classes only implement relevant methods
// 2. No "not supported" exceptions
// 3. Changes to one interface don't affect unrelated classes
// 4. Clear understanding of what a class can do
// 5. Easy to test - mock only what you need

} // namespace isp_good
