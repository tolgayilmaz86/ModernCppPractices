#pragma once

#include "Testable.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <map>

// Singleton registry that holds all sample factories
class SampleRegistry {
public:
    using FactoryFunc = std::function<std::unique_ptr<Testable>()>;

    // Get the singleton instance
    static SampleRegistry& instance() {
        static SampleRegistry registry;
        return registry;
    }

    // Register a sample factory with a name and order
    void registerSample(const std::string& name, int order, FactoryFunc factory) {
        samples_[order] = {name, std::move(factory)};
    }

    // Create all registered samples in order
    std::vector<std::unique_ptr<Testable>> createAll() {
        std::vector<std::unique_ptr<Testable>> result;
        for (auto& [order, entry] : samples_) {
            result.push_back(entry.factory());
        }
        return result;
    }

    // Get sample count
    size_t count() const { return samples_.size(); }

private:
    struct SampleEntry {
        std::string name;
        FactoryFunc factory;
    };

    std::map<int, SampleEntry> samples_;  // Ordered by key

    SampleRegistry() = default;
    SampleRegistry(const SampleRegistry&) = delete;
    SampleRegistry& operator=(const SampleRegistry&) = delete;
};

// Helper class for auto-registration at static initialization time
template <typename T>
class SampleRegistrar {
public:
    SampleRegistrar(const std::string& name, int order) {
        SampleRegistry::instance().registerSample(name, order, []() {
            return std::make_unique<T>();
        });
    }
};

// Macro for easy registration - place at the end of each .cpp file
#define REGISTER_SAMPLE(ClassName, DisplayName, Order) \
    static SampleRegistrar<ClassName> registrar_##ClassName(DisplayName, Order)
