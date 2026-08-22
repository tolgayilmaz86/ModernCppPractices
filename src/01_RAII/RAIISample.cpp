#include "RAIISample.hpp"
#include "SampleRegistry.hpp"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

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

// --- Stand-in for a legacy C driver header we are not allowed to change ---
// The API is the classic acquire/release pair plus a (pointer, length) call.
using DeviceHandle = void *;

struct LegacyDevice {
  int id;
};

int c_driver_init(DeviceHandle *handle) {
  if (!handle) {
    return -1;
  }
  auto *device = new LegacyDevice{42}; // stands in for malloc() in real C code
  *handle = device;
  std::cout << "[C driver] init() -> device #" << device->id << std::endl;
  return 0;
}

int c_driver_process(DeviceHandle handle, const std::uint8_t *buffer, int len) {
  if (!handle || !buffer || len <= 0) {
    return -1;
  }
  auto *device = static_cast<LegacyDevice *>(handle);
  unsigned checksum = 0;
  for (int i = 0; i < len; ++i) {
    checksum += static_cast<unsigned>(buffer[i]);
  }
  std::cout << "[C driver] process() -> device #" << device->id << ", " << len
            << " bytes, checksum = " << checksum << std::endl;
  return 0;
}

int c_driver_deinit(DeviceHandle handle) {
  if (!handle) {
    return -1;
  }
  auto *device = static_cast<LegacyDevice *>(handle);
  std::cout << "[C driver] deinit() -> device #" << device->id << std::endl;
  delete device;
  return 0;
}

// RAII wrapper around the C driver: init() in the constructor, deinit() in the
// destructor, and std::span instead of a raw (pointer, length) pair.
class DeviceWrapper {
private:
  DeviceHandle handle = nullptr;

public:
  // Constructor: Acquire resource (initialize the device)
  DeviceWrapper() {
    if (c_driver_init(&handle) != 0) {
      throw std::runtime_error("Failed to initialize C driver");
    }
  }

  // Destructor: Release resource (deinitialize the device)
  ~DeviceWrapper() {
    if (handle) {
      c_driver_deinit(handle);
    }
  }

  // A handle must have exactly one owner - copying would deinit() it twice
  DeviceWrapper(const DeviceWrapper &) = delete;
  DeviceWrapper &operator=(const DeviceWrapper &) = delete;

  // Moving transfers ownership and leaves the source with a null handle
  DeviceWrapper(DeviceWrapper &&other) noexcept
      : handle(std::exchange(other.handle, nullptr)) {}
  DeviceWrapper &operator=(DeviceWrapper &&other) noexcept {
    if (this != &other) {
      if (handle) {
        c_driver_deinit(handle);
      }
      handle = std::exchange(other.handle, nullptr);
    }
    return *this;
  }

  // std::span carries the pointer and the length together, so the caller can
  // never hand the C API a mismatched (buffer, len) pair. It is a non-owning
  // view - the caller still owns the bytes.
  int process(std::span<const std::uint8_t> buffer) {
    if (!handle) {
      return -1;
    }
    return c_driver_process(handle, buffer.data(),
                            static_cast<int>(buffer.size()));
  }
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

  // Example 3: RAII wrapper around a legacy C driver, using std::span
  std::cout << "\n=== Example 3: Legacy C Driver Wrapper (RAII + std::span) ==="
            << std::endl;
  try {
    // c_driver_init() runs here
    DeviceWrapper device;

    std::vector<std::uint8_t> packet{0x01, 0x02, 0x03, 0x04, 0x05};

    // The vector converts to a span implicitly - no length to pass by hand
    if (device.process(packet) != 0) {
      throw std::runtime_error("C driver processing failed");
    }

    // A subspan is just another view: no copy, no pointer arithmetic
    if (device.process(std::span{packet}.subspan(2)) != 0) {
      throw std::runtime_error("C driver processing failed");
    }

    // The driver rejects an empty payload, so this one throws...
    std::vector<std::uint8_t> emptyPacket;
    if (device.process(emptyPacket) != 0) {
      throw std::runtime_error("C driver processing failed on empty payload");
    }

    std::cout << "This line is never reached." << std::endl;
  } catch (const std::exception &e) {
    // ...and c_driver_deinit() has already run during stack unwinding
    std::cerr << "Error: " << e.what() << std::endl;
  }
  std::cout << "Device released automatically - no manual deinit() call."
            << std::endl;

  std::cout << "\nAll RAII examples completed." << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(RAIISample, "RAII", 1);