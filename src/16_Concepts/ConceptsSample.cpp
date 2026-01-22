#include "ConceptsSample.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <type_traits>
#include <concepts>
#include <numeric>
#include <ranges>
#include <iterator>
#include <algorithm>

// ============================================================================
// C++20 Concepts: A Modern Alternative to SFINAE
// ============================================================================
// Concepts provide a cleaner, more readable way to constrain templates.
// They replace verbose SFINAE patterns with declarative constraints.

// ============================================================================
// Part 1: Standard Library Concepts
// ============================================================================

// Using standard concepts from <concepts>
template <std::integral T>
T add_integers(T a, T b) {
    return a + b;
}

template <std::floating_point T>
T add_floats(T a, T b) {
    return a + b;
}

// std::convertible_to concept
template <typename From, typename To>
    requires std::convertible_to<From, To>
To safe_convert(From value) {
    return static_cast<To>(value);
}

// ============================================================================
// Part 2: Custom Concepts - Basic Syntax
// ============================================================================

// Concept using type traits
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// Concept using requires expression
template <typename T>
concept Printable = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream&>;
};

// Concept combining multiple requirements
template <typename T>
concept Container = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};

// Concept for random access containers (more constrained)
template <typename T>
concept RandomAccessContainer = Container<T> && requires(T t, std::size_t i) {
    { t[i] } -> std::same_as<typename T::reference>;
};

// ============================================================================
// Part 3: Concept-Constrained Functions
// ============================================================================

// Method 1: Concept as template parameter constraint
template <Numeric T>
T double_value(T value) {
    return value * 2;
}

// Method 2: Trailing requires clause
template <typename T>
    requires Numeric<T>
T triple_value(T value) {
    return value * 3;
}

// Method 3: Abbreviated function template with auto
void print_any(const Printable auto& value) {
    std::cout << value << std::endl;
}

// Method 4: Constrained auto return type
Numeric auto compute_sum(Numeric auto a, Numeric auto b) {
    return a + b;
}

// ============================================================================
// Part 4: Overloading with Concepts
// ============================================================================

// More constrained overload wins (subsumption)
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

template <typename T>
concept Summable = Addable<T> && requires(T t) {
    { t * 2 } -> std::convertible_to<T>;
};

// Less constrained - matches types that are just Addable
template <Addable T>
T combine(T a, T b) {
    std::cout << "[Addable] Combining via addition" << std::endl;
    return a + b;
}

// More constrained - preferred for types that are also Summable
template <Summable T>
T combine(T a, T b) requires std::is_arithmetic_v<T> {
    std::cout << "[Summable+Arithmetic] Combining with multiplication check" << std::endl;
    return a + b;
}

// ============================================================================
// Part 5: Requires Expressions - Detailed Syntax
// ============================================================================

// Simple requirement: expression must be valid
template <typename T>
concept HasToString = requires(T t) {
    t.to_string();  // Simple requirement
};

// Type requirement: associated type must exist
template <typename T>
concept HasValueType = requires {
    typename T::value_type;  // Type requirement
};

// Compound requirement: expression + return type constraint
template <typename T>
concept Hashable = requires(T t) {
    { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

// Nested requirement: additional predicate
template <typename T>
concept SmallNumeric = requires(T t) {
    requires sizeof(T) <= 4;  // Nested requirement
    requires std::is_arithmetic_v<T>;
};

// ============================================================================
// Part 6: Comparing SFINAE vs Concepts
// ============================================================================

// === SFINAE Approach (verbose, hard to read) ===
template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

template <typename T>
typename std::enable_if<has_size<T>::value, std::size_t>::type
get_size_sfinae(const T& container) {
    return container.size();
}

template <typename T>
typename std::enable_if<!has_size<T>::value, std::size_t>::type
get_size_sfinae(const T&) {
    return 1;  // Non-containers have "size" 1
}

// === Concepts Approach (clean, readable) ===
template <typename T>
concept Sizable = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

std::size_t get_size_concepts(const Sizable auto& container) {
    return container.size();
}

std::size_t get_size_concepts(const auto&) {
    return 1;  // Fallback for non-sizable types
}

// ============================================================================
// Part 7: Concepts with Classes
// ============================================================================

// Concept-constrained class template
template <Numeric T>
class Calculator {
private:
    T value_;
public:
    explicit Calculator(T initial) : value_(initial) {}
    
    void add(T amount) { value_ += amount; }
    void multiply(T factor) { value_ *= factor; }
    T get() const { return value_; }
};

// Class with concept-constrained member functions
class FlexibleContainer {
private:
    std::vector<int> data_;
public:
    // Only enable for random access iterators
    template <std::random_access_iterator Iter>
    void assign_from(Iter begin, Iter end) {
        data_.assign(begin, end);
        std::cout << "Assigned from random access iterators" << std::endl;
    }
    
    // Fallback for input iterators
    template <std::input_iterator Iter>
        requires (!std::random_access_iterator<Iter>)
    void assign_from(Iter begin, Iter end) {
        data_.clear();
        for (auto it = begin; it != end; ++it) {
            data_.push_back(*it);
        }
        std::cout << "Assigned from input iterators (slower path)" << std::endl;
    }
    
    void print() const {
        std::cout << "Contents: ";
        for (int v : data_) std::cout << v << " ";
        std::cout << std::endl;
    }
};

// ============================================================================
// Part 8: Composing Concepts
// ============================================================================

// Concept conjunction (AND)
template <typename T>
concept IntegralAndSigned = std::integral<T> && std::signed_integral<T>;

// Concept disjunction (OR) - using requires
template <typename T>
concept NumberLike = std::integral<T> || std::floating_point<T>;

// Complex composed concept
template <typename T>
concept SortableContainer = Container<T> && requires(T t) {
    requires std::totally_ordered<typename T::value_type>;
    { std::begin(t) } -> std::random_access_iterator;
};

template <SortableContainer T>
void sort_container(T& container) {
    std::sort(container.begin(), container.end());
    std::cout << "Sorted container with " << container.size() << " elements" << std::endl;
}

// ============================================================================
// Part 9: Concepts with Variadic Templates
// ============================================================================

// All types must satisfy concept
template <typename... Args>
    requires (Numeric<Args> && ...)
auto sum_all(Args... args) {
    return (args + ...);
}

// At least one type must satisfy concept
template <typename... Args>
    requires (Printable<Args> || ...)
void print_first_printable(Args&&... args) {
    // Use fold expression to print first printable
    ((std::cout << args << std::endl, true) || ...);
}

// ============================================================================
// Demonstration Functions
// ============================================================================

void demonstrate_standard_concepts() {
    std::cout << "\n=== Standard Library Concepts ===" << std::endl;
    
    // std::integral
    auto int_sum = add_integers(10, 20);
    std::cout << "add_integers(10, 20) = " << int_sum << std::endl;
    
    // std::floating_point
    auto float_sum = add_floats(3.14, 2.86);
    std::cout << "add_floats(3.14, 2.86) = " << float_sum << std::endl;
    
    // std::convertible_to
    double d = safe_convert<int, double>(42);
    std::cout << "safe_convert<int, double>(42) = " << d << std::endl;
}

void demonstrate_custom_concepts() {
    std::cout << "\n=== Custom Concepts ===" << std::endl;
    
    // Numeric concept
    std::cout << "double_value(21) = " << double_value(21) << std::endl;
    std::cout << "triple_value(7.0) = " << triple_value(7.0) << std::endl;
    
    // Printable concept
    std::cout << "print_any with string: ";
    print_any(std::string("Hello, Concepts!"));
    
    // compute_sum with auto deduction
    auto result = compute_sum(100, 200);
    std::cout << "compute_sum(100, 200) = " << result << std::endl;
}

void demonstrate_concept_overloading() {
    std::cout << "\n=== Concept Overloading (Subsumption) ===" << std::endl;
    
    // String satisfies Addable but not the arithmetic constraint
    std::string s1 = "Hello, ";
    std::string s2 = "World!";
    auto str_result = combine(s1, s2);
    std::cout << "Result: " << str_result << std::endl;
    
    // int satisfies both Addable and Summable+arithmetic
    int n1 = 10, n2 = 20;
    auto int_result = combine(n1, n2);
    std::cout << "Result: " << int_result << std::endl;
}

void demonstrate_sfinae_vs_concepts() {
    std::cout << "\n=== SFINAE vs Concepts Comparison ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    int single = 42;
    
    std::cout << "SFINAE get_size:" << std::endl;
    std::cout << "  vector size: " << get_size_sfinae(vec) << std::endl;
    std::cout << "  int size: " << get_size_sfinae(single) << std::endl;
    
    std::cout << "Concepts get_size:" << std::endl;
    std::cout << "  vector size: " << get_size_concepts(vec) << std::endl;
    std::cout << "  int size: " << get_size_concepts(single) << std::endl;
    
    std::cout << "\nCode comparison:" << std::endl;
    std::cout << "SFINAE:   template<typename T> enable_if<has_size<T>::value, size_t>::type" << std::endl;
    std::cout << "Concepts: size_t get_size(const Sizable auto& container)" << std::endl;
}

void demonstrate_class_constraints() {
    std::cout << "\n=== Concept-Constrained Classes ===" << std::endl;
    
    Calculator<int> int_calc(100);
    int_calc.add(50);
    int_calc.multiply(2);
    std::cout << "Calculator<int> result: " << int_calc.get() << std::endl;
    
    Calculator<double> double_calc(3.14);
    double_calc.multiply(2);
    std::cout << "Calculator<double> result: " << double_calc.get() << std::endl;
    
    // FlexibleContainer with different iterator types
    FlexibleContainer fc;
    
    std::vector<int> vec = {1, 2, 3};
    fc.assign_from(vec.begin(), vec.end());
    fc.print();
    
    std::list<int> lst = {4, 5, 6};
    fc.assign_from(lst.begin(), lst.end());
    fc.print();
}

void demonstrate_composed_concepts() {
    std::cout << "\n=== Composed Concepts ===" << std::endl;
    
    std::vector<int> sortable = {5, 2, 8, 1, 9};
    std::cout << "Before sort: ";
    for (int v : sortable) std::cout << v << " ";
    std::cout << std::endl;
    
    sort_container(sortable);
    
    std::cout << "After sort: ";
    for (int v : sortable) std::cout << v << " ";
    std::cout << std::endl;
}

void demonstrate_variadic_concepts() {
    std::cout << "\n=== Variadic Template Concepts ===" << std::endl;
    
    auto total = sum_all(1, 2, 3, 4, 5);
    std::cout << "sum_all(1, 2, 3, 4, 5) = " << total << std::endl;
    
    auto mixed = sum_all(1, 2.5, 3, 4.5);
    std::cout << "sum_all(1, 2.5, 3, 4.5) = " << mixed << std::endl;
}

void demonstrate_error_messages() {
    std::cout << "\n=== Better Error Messages ===" << std::endl;
    std::cout << "Concepts provide clearer compile errors:" << std::endl;
    std::cout << std::endl;
    std::cout << "SFINAE error example:" << std::endl;
    std::cout << "  'no matching function for call to add_integers'" << std::endl;
    std::cout << "  'candidate template ignored: substitution failure'" << std::endl;
    std::cout << std::endl;
    std::cout << "Concepts error example:" << std::endl;
    std::cout << "  'constraints not satisfied'" << std::endl;
    std::cout << "  'the concept std::integral<T> was not satisfied'" << std::endl;
    std::cout << std::endl;
    std::cout << "Concepts tell you WHAT constraint failed, not just THAT it failed!" << std::endl;
}

// ============================================================================
// Main Run Function
// ============================================================================

#include "SampleRegistry.hpp"

void ConceptsSample::run() {
    std::cout << "Running C++20 Concepts Sample..." << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Concepts provide a cleaner alternative to SFINAE for" << std::endl;
    std::cout << "constraining templates with better error messages." << std::endl;

    demonstrate_standard_concepts();
    demonstrate_custom_concepts();
    demonstrate_concept_overloading();
    demonstrate_sfinae_vs_concepts();
    demonstrate_class_constraints();
    demonstrate_composed_concepts();
    demonstrate_variadic_concepts();
    demonstrate_error_messages();

    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "1. Concepts replace verbose SFINAE with clean syntax" << std::endl;
    std::cout << "2. Use standard concepts from <concepts> when possible" << std::endl;
    std::cout << "3. requires expressions define custom constraints" << std::endl;
    std::cout << "4. More constrained overloads are preferred (subsumption)" << std::endl;
    std::cout << "5. Concepts work with classes, functions, and lambdas" << std::endl;
    std::cout << "6. Error messages are dramatically improved" << std::endl;

    std::cout << "\nC++20 Concepts demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(ConceptsSample, "C++20 Concepts", 16);
