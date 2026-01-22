#include "RAIISample.hpp"
#include "SampleRegistry.hpp"
#include <iostream>
#include <fstream>
#include <string>

// RAII File Wrapper Example
class File {
private:
    std::fstream fileStream;
    std::string filename;

public:
    // Constructor: Acquire resource (open file)
    File(const std::string& fname, std::ios_base::openmode mode = std::ios::in | std::ios::out)
        : filename(fname) {
        fileStream.open(filename, mode);
        if (!fileStream.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        std::cout << "File opened: " << filename << std::endl;
    }

    // Destructor: Release resource (close file)
    ~File() {
        if (fileStream.is_open()) {
            fileStream.close();
            std::cout << "File closed: " << filename << std::endl;
        }
    }

    // Delete copy constructor and assignment to prevent multiple ownership
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // Allow move operations
    File(File&& other) noexcept : fileStream(std::move(other.fileStream)), filename(std::move(other.filename)) {
        other.filename.clear();
    }
    File& operator=(File&& other) noexcept {
        if (this != &other) {
            fileStream = std::move(other.fileStream);
            filename = std::move(other.filename);
            other.filename.clear();
        }
        return *this;
    }

    // Utility methods
    void write(const std::string& data) {
        if (fileStream.is_open()) {
            fileStream << data << std::endl;
        }
    }

    std::string readLine() {
        std::string line;
        if (fileStream.is_open() && std::getline(fileStream, line)) {
            return line;
        }
        return "";
    }

    bool isOpen() const {
        return fileStream.is_open();
    }
};

void RAIISample::run() {
    std::cout << "Running RAII Sample..." << std::endl;
    try {
        // File is automatically opened here
        File myFile("example.txt", std::ios::out);

        myFile.write("Hello, RAII!");
        myFile.write("This file will be automatically closed.");
        myFile.write("No need to manually call close().");

        // File is automatically closed when myFile goes out of scope
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "File operations completed. File is now closed." << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(RAIISample, "RAII", 1);