#include "RAIISample.hpp"
#include "SampleRegistry.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
namespace {

// RAII File Wrapper Example
class File {
private:
  std::fstream fileStream;
  std::string filename;

public:
  // Constructor: Acquire resource (open file)
  File(const std::string &fname,
       std::ios_base::openmode mode = std::ios::in | std::ios::out)
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
  File(const File &) = delete;
  File &operator=(const File &) = delete;

  // Allow move operations
  File(File &&other) noexcept
      : fileStream(std::move(other.fileStream)),
        filename(std::move(other.filename)) {
    other.filename.clear();
  }
  File &operator=(File &&other) noexcept {
    if (this != &other) {
      fileStream = std::move(other.fileStream);
      filename = std::move(other.filename);
      other.filename.clear();
    }
    return *this;
  }

  // Utility methods
  void write(const std::string &data) {
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

  bool isOpen() const { return fileStream.is_open(); }
};

// RAII Timer Example - Automatically measures execution time
class ScopedTimer {
private:
  std::string operationName;
  std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

public:
  // Constructor: Start timing
  explicit ScopedTimer(const std::string &name)
      : operationName(name),
        startTime(std::chrono::high_resolution_clock::now()) {
    std::cout << "[Timer] Starting: " << operationName << std::endl;
  }

  // Destructor: Stop timing and report
  ~ScopedTimer() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);
    std::cout << "[Timer] " << operationName << " completed in "
              << duration.count() << " ms" << std::endl;
  }

  // Non-copyable, non-movable (each timer is unique)
  ScopedTimer(const ScopedTimer &) = delete;
  ScopedTimer &operator=(const ScopedTimer &) = delete;
  ScopedTimer(ScopedTimer &&) = delete;
  ScopedTimer &operator=(ScopedTimer &&) = delete;
};

} // end anonymous namespace

void RAIISample::run() {
  std::cout << "Running RAII Sample..." << std::endl;

  // Example 1: File RAII Wrapper
  std::cout << "\n=== Example 1: File RAII Wrapper ===" << std::endl;
  try {
    // File is automatically opened here
    File myFile("example.txt", std::ios::out);

    myFile.write("Hello, RAII!");
    myFile.write("This file will be automatically closed.");
    myFile.write("No need to manually call close().");

    // File is automatically closed when myFile goes out of scope
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  std::cout << "File operations completed. File is now closed." << std::endl;

  // Example 2: ScopedTimer RAII Wrapper
  std::cout << "\n=== Example 2: ScopedTimer RAII Wrapper ===" << std::endl;
  {
    ScopedTimer timer("Simulated work");

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Doing some work..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "More work..." << std::endl;

    // Timer automatically prints duration when going out of scope
  }

  // Nested timers example
  std::cout << "\n=== Nested Timers Demo ===" << std::endl;
  {
    ScopedTimer outerTimer("Outer operation");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
      ScopedTimer innerTimer("Inner operation");
      std::this_thread::sleep_for(std::chrono::milliseconds(75));
      // innerTimer reports first (LIFO order - like a stack!)
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    // outerTimer reports after inner is done
  }

  std::cout << "\nAll RAII examples completed." << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(RAIISample, "RAII", 1);