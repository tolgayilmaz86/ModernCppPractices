#include "RuleOfFiveSample.hpp"
#include <iostream>
#include <memory>
#include <cstring>
#include <fstream>
#include <vector>

// Example 1: Class managing heap memory (C-style)
class HeapResource {
private:
    char* data_;
    size_t size_;

public:
    // Constructor
    explicit HeapResource(size_t size) : size_(size) {
        data_ = new char[size_];
        std::memset(data_, 0, size_);
        std::cout << "HeapResource: Allocated " << size_ << " bytes" << std::endl;
    }

    // Destructor
    ~HeapResource() {
        delete[] data_;
        std::cout << "HeapResource: Deallocated " << size_ << " bytes" << std::endl;
    }

    // Copy constructor (Rule of Five)
    HeapResource(const HeapResource& other) : size_(other.size_) {
        data_ = new char[size_];
        std::memcpy(data_, other.data_, size_);
        std::cout << "HeapResource: Deep copied " << size_ << " bytes" << std::endl;
    }

    // Copy assignment operator (Rule of Five)
    HeapResource& operator=(const HeapResource& other) {
        if (this != &other) {
            // Clean up existing resource
            delete[] data_;

            // Copy the resource
            size_ = other.size_;
            data_ = new char[size_];
            std::memcpy(data_, other.data_, size_);
            std::cout << "HeapResource: Deep copy assigned " << size_ << " bytes" << std::endl;
        }
        return *this;
    }

    // Move constructor (Rule of Five)
    HeapResource(HeapResource&& other) noexcept : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "HeapResource: Moved " << size_ << " bytes" << std::endl;
    }

    // Move assignment operator (Rule of Five)
    HeapResource& operator=(HeapResource&& other) noexcept {
        if (this != &other) {
            // Clean up existing resource
            delete[] data_;

            // Move the resource
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
            std::cout << "HeapResource: Move assigned " << size_ << " bytes" << std::endl;
        }
        return *this;
    }

    // Utility methods
    void setData(size_t index, char value) {
        if (index < size_) {
            data_[index] = value;
        }
    }

    char getData(size_t index) const {
        return (index < size_) ? data_[index] : '\0';
    }

    size_t size() const { return size_; }
};

// Example 2: Class managing file handles
class FileResource {
private:
    std::fstream* file_;
    std::string filename_;

public:
    // Constructor
    explicit FileResource(const std::string& filename)
        : filename_(filename) {
        file_ = new std::fstream(filename, std::ios::in | std::ios::out | std::ios::trunc);
        if (file_->is_open()) {
            std::cout << "FileResource: Opened file '" << filename_ << "'" << std::endl;
        } else {
            std::cout << "FileResource: Failed to open file '" << filename_ << "'" << std::endl;
        }
    }

    // Destructor
    ~FileResource() {
        if (file_ && file_->is_open()) {
            file_->close();
        }
        delete file_;
        std::cout << "FileResource: Closed file '" << filename_ << "'" << std::endl;
    }

    // Copy constructor (Rule of Five)
    FileResource(const FileResource& other) : filename_(other.filename_ + "_copy") {
        file_ = new std::fstream(filename_, std::ios::in | std::ios::out | std::ios::trunc);
        if (file_->is_open()) {
            std::cout << "FileResource: Copy created file '" << filename_ << "'" << std::endl;
        }
    }

    // Copy assignment operator (Rule of Five)
    FileResource& operator=(const FileResource& other) {
        if (this != &other) {
            // Clean up existing resource
            if (file_ && file_->is_open()) {
                file_->close();
            }
            delete file_;

            // Copy the resource
            filename_ = other.filename_ + "_assigned";
            file_ = new std::fstream(filename_, std::ios::in | std::ios::out | std::ios::trunc);
            std::cout << "FileResource: Copy assigned file '" << filename_ << "'" << std::endl;
        }
        return *this;
    }

    // Move constructor (Rule of Five)
    FileResource(FileResource&& other) noexcept
        : file_(other.file_), filename_(std::move(other.filename_)) {
        other.file_ = nullptr;
        std::cout << "FileResource: Moved file '" << filename_ << "'" << std::endl;
    }

    // Move assignment operator (Rule of Five)
    FileResource& operator=(FileResource&& other) noexcept {
        if (this != &other) {
            // Clean up existing resource
            if (file_ && file_->is_open()) {
                file_->close();
            }
            delete file_;

            // Move the resource
            file_ = other.file_;
            filename_ = std::move(other.filename_);
            other.file_ = nullptr;
            std::cout << "FileResource: Move assigned file '" << filename_ << "'" << std::endl;
        }
        return *this;
    }

    // Utility methods
    bool writeData(const std::string& data) {
        if (file_ && file_->is_open()) {
            *file_ << data << std::endl;
            return true;
        }
        return false;
    }

    std::string readData() {
        if (file_ && file_->is_open()) {
            std::string line;
            std::getline(*file_, line);
            return line;
        }
        return "";
    }

    const std::string& filename() const { return filename_; }
};

// Example 3: Class that demonstrates the problem when Rule of Five is not followed
class BrokenResource {
private:
    int* data_;

public:
    // Constructor
    explicit BrokenResource(int value) {
        data_ = new int(value);
        std::cout << "BrokenResource: Created with value " << *data_ << std::endl;
    }

    // Destructor - only this one is implemented!
    ~BrokenResource() {
        delete data_;
        std::cout << "BrokenResource: Destroyed" << std::endl;
    }

    // No copy constructor, copy assignment, move constructor, or move assignment!
    // This will cause problems...

    int getValue() const { return *data_; }
    void setValue(int value) { *data_ = value; }
};

// Example 4: Proper implementation following Rule of Five
class ProperResource {
private:
    int* data_;

public:
    // Constructor
    explicit ProperResource(int value) {
        data_ = new int(value);
        std::cout << "ProperResource: Created with value " << *data_ << std::endl;
    }

    // Destructor
    ~ProperResource() {
        delete data_;
        std::cout << "ProperResource: Destroyed" << std::endl;
    }

    // Copy constructor
    ProperResource(const ProperResource& other) {
        data_ = new int(*other.data_);
        std::cout << "ProperResource: Deep copied with value " << *data_ << std::endl;
    }

    // Copy assignment operator
    ProperResource& operator=(const ProperResource& other) {
        if (this != &other) {
            delete data_;
            data_ = new int(*other.data_);
            std::cout << "ProperResource: Deep copy assigned with value " << *data_ << std::endl;
        }
        return *this;
    }

    // Move constructor
    ProperResource(ProperResource&& other) noexcept : data_(other.data_) {
        other.data_ = nullptr;
        std::cout << "ProperResource: Moved" << std::endl;
    }

    // Move assignment operator
    ProperResource& operator=(ProperResource&& other) noexcept {
        if (this != &other) {
            delete data_;
            data_ = other.data_;
            other.data_ = nullptr;
            std::cout << "ProperResource: Move assigned" << std::endl;
        }
        return *this;
    }

    int getValue() const { return *data_; }
    void setValue(int value) { *data_ = value; }
};

#include "SampleRegistry.hpp"

void RuleOfFiveSample::run() {
    std::cout << "Running Rule of Five Sample..." << std::endl;

    // Demonstrate HeapResource (proper Rule of Five implementation)
    std::cout << "\n=== HeapResource (Proper Rule of Five) ===" << std::endl;
    HeapResource heap1(10);
    heap1.setData(0, 'A');
    heap1.setData(1, 'B');
    std::cout << "heap1[0] = " << heap1.getData(0) << ", heap1[1] = " << heap1.getData(1) << std::endl;

    // Copy construction
    HeapResource heap2 = heap1;
    std::cout << "heap2[0] = " << heap2.getData(0) << ", heap2[1] = " << heap2.getData(1) << std::endl;

    // Copy assignment
    HeapResource heap3(5);
    heap3 = heap1;
    std::cout << "heap3[0] = " << heap3.getData(0) << ", heap3[1] = " << heap3.getData(1) << std::endl;

    // Move construction
    HeapResource heap4 = std::move(heap2);

    // Move assignment
    HeapResource heap5(3);
    heap5 = std::move(heap3);

    // Demonstrate FileResource
    std::cout << "\n=== FileResource (Proper Rule of Five) ===" << std::endl;
    FileResource file1("test1.txt");
    file1.writeData("Hello from file1");

    // Copy construction
    FileResource file2 = file1;
    file2.writeData("Hello from file2");

    // Copy assignment
    FileResource file3("test3.txt");
    file3 = file1;
    file3.writeData("Hello from file3");

    // Move operations
    FileResource file4 = std::move(file2);
    FileResource file5("test5.txt");
    file5 = std::move(file3);

    // Demonstrate the problem with BrokenResource
    std::cout << "\n=== BrokenResource (Violates Rule of Five) ===" << std::endl;
    std::cout << "This will demonstrate undefined behavior!" << std::endl;

    try {
        BrokenResource broken1(42);
        std::cout << "broken1 value: " << broken1.getValue() << std::endl;

        // This will cause double deletion when both objects are destroyed!
        // BrokenResource broken2 = broken1;  // Shallow copy - disaster!

        std::cout << "Avoiding the copy to prevent crash..." << std::endl;
    } catch (...) {
        std::cout << "Exception caught!" << std::endl;
    }

    // Demonstrate proper implementation
    std::cout << "\n=== ProperResource (Follows Rule of Five) ===" << std::endl;
    ProperResource proper1(100);
    std::cout << "proper1 value: " << proper1.getValue() << std::endl;

    // Copy construction - safe!
    ProperResource proper2 = proper1;
    std::cout << "proper2 value: " << proper2.getValue() << std::endl;

    // Copy assignment - safe!
    ProperResource proper3(50);
    proper3 = proper1;
    std::cout << "proper3 value: " << proper3.getValue() << std::endl;

    // Move construction - efficient!
    ProperResource proper4 = std::move(proper2);

    // Move assignment - efficient!
    ProperResource proper5(25);
    proper5 = std::move(proper3);

    std::cout << "\nRule of Five demonstration completed!" << std::endl;
    std::cout << "Key takeaways:" << std::endl;
    std::cout << "- If a class manages resources, implement all 5 special member functions" << std::endl;
    std::cout << "- Copy operations should perform deep copies" << std::endl;
    std::cout << "- Move operations should leave source in valid but unspecified state" << std::endl;
    std::cout << "- Violating the rule leads to resource leaks, double deletion, or crashes" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(RuleOfFiveSample, "Rule of Five", 5);