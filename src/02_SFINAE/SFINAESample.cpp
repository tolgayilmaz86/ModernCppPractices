#include "SFINAESample.hpp"
#include "SampleRegistry.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <concepts>

// SFINAE Example: Different approaches to conditional compilation

// ============================================================================
// Approach 1: Traditional SFINAE with std::enable_if
// ============================================================================

// Helper for detecting if a type has begin() method (is container-like)
template <typename T>
class has_begin {
private:
    // Compiler interprets this as "if U has begin(), this overload is chosen"
    // decltype is used to check for the existence of begin() and true_type is returned
    template <typename U>
    static auto test(int) -> decltype(std::declval<U>().begin(), std::true_type());

    template <typename>
    static std::false_type test(...); // Fallback

public:
    // Extract the boolean result from the SFINAE test for T
    static constexpr bool value = decltype(test<T>(0))::value;
};

// Traditional SFINAE: Function that works only for containers
// Compilers read it as "enable this function if class T has begin()"
template <typename T>
typename std::enable_if<has_begin<T>::value, void>::type // void represents return type
print_container_enable_if(const T& container) {
    std::cout << "[SFINAE] Container contents: ";
    for (const auto& item : container) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

// Traditional SFINAE: Function that works only for non-containers
template <typename T>
typename std::enable_if<!has_begin<T>::value, void>::type // void represents return type
print_container_enable_if(const T& value) {
    std::cout << "[SFINAE] Single value: " << value << std::endl;
}

// ============================================================================
// Approach 2: C++17 if constexpr
// ============================================================================

// Single function that handles both cases with compile-time branching
template <typename T>
void print_container_constexpr(const T& value) {
    if constexpr (has_begin<T>::value) {
        std::cout << "[if constexpr] Container contents: ";
        for (const auto& item : value) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "[if constexpr] Single value: " << value << std::endl;
    }
}

// ============================================================================
// Approach 3: C++20 Concepts
// ============================================================================

// Define concepts for type constraints
template <typename T>
concept Container = requires(T t) {
    t.begin();
    t.end();
};

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// Concept-based function overloading
void print_container_concepts(const Container auto& container) {
    std::cout << "[Concepts] Container contents: ";
    for (const auto& item : container) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

// auto parameter can be constrained by concepts
void print_container_concepts(const auto& value) {
    std::cout << "[Concepts] Single value: " << value << std::endl;
}

// Arithmetic operations with concepts
auto double_value_concepts(const Arithmetic auto& value) {
    std::cout << "[Concepts] Doubling arithmetic: " << value << " -> " << (value * 2) << std::endl;
    return value * 2;
}

// Overload for non-arithmetic types
auto double_value_concepts(const auto& value) {
    std::cout << "[Concepts] Converting to string: " << value << " -> \"" << value << value << "\"" << std::endl;
    return std::string(value) + std::string(value);
}

// ============================================================================

// Traditional SFINAE arithmetic functions (for comparison)
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
double_value(T value) {
    std::cout << "[SFINAE] Doubling arithmetic value: " << value << " -> " << (value * 2) << std::endl;
    return value * 2;
}

template <typename T>
typename std::enable_if<!std::is_arithmetic<T>::value, std::string>::type
double_value(const T& value) {
    std::cout << "[SFINAE] Converting non-arithmetic to string: " << value << " -> \"" << value << value << "\"" << std::endl;
    return std::string(value) + std::string(value);
}

// ============================================================================
// Comparison Demonstration
// ============================================================================

void demonstrateSFINAE() {
    std::cout << "=== SFINAE Evolution: Three Approaches ===\n" << std::endl;

    std::vector<int> vec = {1, 2, 3, 4, 5};
    int single_value = 42;
    std::string str = "hello";

    // Demonstrate all three approaches with the same data
    std::cout << "1. Traditional SFINAE with std::enable_if:" << std::endl;
    print_container_enable_if(vec);
    print_container_enable_if(single_value);
    print_container_enable_if(str);

    std::cout << "\n2. C++17 if constexpr:" << std::endl;
    print_container_constexpr(vec);
    print_container_constexpr(single_value);
    print_container_constexpr(str);

    std::cout << "\n3. C++20 Concepts:" << std::endl;
    print_container_concepts(vec);
    print_container_concepts(single_value);
    print_container_concepts(str);

    std::cout << "\n4. Arithmetic operations comparison:" << std::endl;

    // Traditional SFINAE arithmetic
    std::cout << "Traditional SFINAE arithmetic:" << std::endl;
    double_value(5);      // from earlier implementation
    double_value(3.14);
    double_value(std::string("abc"));

    // Concepts arithmetic
    std::cout << "\nConcepts arithmetic:" << std::endl;
    double_value_concepts(5);
    double_value_concepts(3.14);
    double_value_concepts(std::string("abc"));

    std::cout << "\n=== Key Differences ===" << std::endl;
    std::cout << "std::enable_if: Creates multiple function overloads, SFINAE discards invalid ones" << std::endl;
    std::cout << "if constexpr: Single function with compile-time conditional compilation" << std::endl;
    std::cout << "Concepts: Clear, readable constraints with better error messages" << std::endl;
    std::cout << "\nAll approaches achieve the same result but with different trade-offs!" << std::endl;
}

void SFINAESample::run() {
    std::cout << "Running SFINAE Sample..." << std::endl;
    demonstrateSFINAE();
}

// Auto-register this sample
REGISTER_SAMPLE(SFINAESample, "SFINAE", 2);