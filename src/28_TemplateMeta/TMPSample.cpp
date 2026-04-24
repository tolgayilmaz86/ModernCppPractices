#include "TMPSample.hpp"
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <tuple>
#include <array>
#include <numeric>
#include <concepts>

// =============================================================================
// 1. COMPILE-TIME VALUES: constexpr functions & recursive templates
// =============================================================================

// Classic TMP: compile-time factorial via recursive struct
template <unsigned N>
struct Factorial {
    static constexpr unsigned value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr unsigned value = 1;
};

// Classic TMP: compile-time Fibonacci
template <unsigned N>
struct Fibonacci {
    static constexpr unsigned value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};
template <> struct Fibonacci<0> { static constexpr unsigned value = 0; };
template <> struct Fibonacci<1> { static constexpr unsigned value = 1; };

// Modern alternative: constexpr function (C++14+)
constexpr unsigned constexpr_factorial(unsigned n) {
    return n == 0 ? 1 : n * constexpr_factorial(n - 1);
}

// Compile-time power
template <int Base, unsigned Exp>
struct Power {
    static constexpr long long value = Base * Power<Base, Exp - 1>::value;
};
template <int Base>
struct Power<Base, 0> {
    static constexpr long long value = 1;
};

void TMPSample::demonstrate_compile_time_values() {
    std::cout << "\n=== Compile-Time Values ===\n";

    // Recursive struct metafunctions
    std::cout << "Factorial<5>::value  = " << Factorial<5>::value  << "\n";  // 120
    std::cout << "Factorial<10>::value = " << Factorial<10>::value << "\n"; // 3628800
    std::cout << "Fibonacci<10>::value = " << Fibonacci<10>::value << "\n"; // 55
    std::cout << "Power<2,10>::value   = " << Power<2,10>::value   << "\n"; // 1024

    // constexpr function — computed at compile time when possible
    constexpr unsigned ct_fact = constexpr_factorial(7);
    std::cout << "constexpr_factorial(7) = " << ct_fact << "\n"; // 5040

    // Verify values are truly compile-time constants
    static_assert(Factorial<5>::value == 120,  "Factorial<5> must be 120");
    static_assert(Fibonacci<10>::value == 55,  "Fibonacci<10> must be 55");
    static_assert(Power<2, 10>::value == 1024, "2^10 must be 1024");
    static_assert(constexpr_factorial(6) == 720, "constexpr factorial(6) must be 720");
    std::cout << "All compile-time assertions passed!\n";
}

// =============================================================================
// 2. TYPE TRAITS — inspecting and transforming types at compile time
// =============================================================================

// Custom type trait: is the type a pointer?
template <typename T>
struct is_raw_pointer : std::false_type {};

template <typename T>
struct is_raw_pointer<T*> : std::true_type {};

// Helper variable template (C++17)
template <typename T>
inline constexpr bool is_raw_pointer_v = is_raw_pointer<T>::value;

// Custom type trait: strip const, volatile, and reference
template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

// Custom integral_constant-based trait: rank of an array
template <typename T>
struct array_rank : std::integral_constant<std::size_t, 0> {};

template <typename T, std::size_t N>
struct array_rank<T[N]> : std::integral_constant<std::size_t, 1 + array_rank<T>::value> {};

void TMPSample::demonstrate_type_traits() {
    std::cout << "\n=== Type Traits ===\n";

    // Standard library traits
    std::cout << "std::is_integral<int>::value       : " << std::is_integral<int>::value       << "\n";
    std::cout << "std::is_integral<double>::value    : " << std::is_integral<double>::value    << "\n";
    std::cout << "std::is_floating_point<float>::value: " << std::is_floating_point<float>::value << "\n";
    std::cout << "std::is_pointer<int*>::value       : " << std::is_pointer<int*>::value       << "\n";
    std::cout << "std::is_reference<int&>::value     : " << std::is_reference<int&>::value     << "\n";
    std::cout << "std::is_const<const int>::value    : " << std::is_const<const int>::value    << "\n";

    // Type transformation traits
    std::cout << "\nType transformations:\n";
    using T1 = std::remove_const_t<const int>;      // int
    using T2 = std::add_pointer_t<double>;            // double*
    using T3 = std::remove_reference_t<int&&>;        // int
    using T4 = std::make_signed_t<unsigned int>;      // int
    std::cout << "  remove_const<const int>   -> int          : "
              << std::is_same_v<T1, int>       << "\n";
    std::cout << "  add_pointer<double>       -> double*      : "
              << std::is_same_v<T2, double*>   << "\n";
    std::cout << "  remove_reference<int&&>   -> int          : "
              << std::is_same_v<T3, int>        << "\n";
    std::cout << "  make_signed<unsigned int> -> int          : "
              << std::is_same_v<T4, int>        << "\n";

    // Custom traits
    std::cout << "\nCustom type traits:\n";
    std::cout << "  is_raw_pointer_v<int>    : " << is_raw_pointer_v<int>    << "\n";
    std::cout << "  is_raw_pointer_v<int*>   : " << is_raw_pointer_v<int*>   << "\n";
    std::cout << "  is_raw_pointer_v<int**>  : " << is_raw_pointer_v<int**>  << "\n";

    using stripped = remove_cvref_t<const int&&>;
    std::cout << "  remove_cvref<const int&&> -> int: "
              << std::is_same_v<stripped, int> << "\n";

    std::cout << "  array_rank<int>      : " << array_rank<int>::value       << "\n"; // 0
    std::cout << "  array_rank<int[3]>   : " << array_rank<int[3]>::value    << "\n"; // 1
    std::cout << "  array_rank<int[2][4]>: " << array_rank<int[2][4]>::value << "\n"; // 2
}

// =============================================================================
// 3. VARIADIC TEMPLATES — arbitrary number of template parameters
// =============================================================================

// Recursive variadic sum (classic TMP style)
template <typename T>
T variadic_sum(T v) { return v; }

template <typename T, typename... Rest>
T variadic_sum(T first, Rest... rest) {
    return first + variadic_sum(static_cast<T>(rest)...);
}

// Print all arguments with type info
template <typename T>
void print_all(const T& v) {
    std::cout << v << "\n";
}
template <typename T, typename... Args>
void print_all(const T& first, const Args&... rest) {
    std::cout << first << ", ";
    print_all(rest...);
}

// Tuple-based heterogeneous container access (index sequence trick)
template <std::size_t... Is, typename Tuple>
void print_tuple_impl(std::index_sequence<Is...>, const Tuple& t) {
    // Fold expression over comma operator
    ((std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
    std::cout << "\n";
}

template <typename... Ts>
void print_tuple(const std::tuple<Ts...>& t) {
    print_tuple_impl(std::index_sequence_for<Ts...>{}, t);
}

void TMPSample::demonstrate_variadic_templates() {
    std::cout << "\n=== Variadic Templates ===\n";

    std::cout << "variadic_sum(1,2,3,4,5)     = " << variadic_sum(1, 2, 3, 4, 5)          << "\n";
    std::cout << "variadic_sum(1.5, 2.5, 3.0) = " << variadic_sum(1.5, 2.5, 3.0)          << "\n";
    std::cout << "variadic_sum(10)             = " << variadic_sum(10)                      << "\n";

    std::cout << "\nprint_all(42, 3.14, \"hello\", true): ";
    print_all(42, 3.14, "hello", true);

    auto t = std::make_tuple(1, 2.5, std::string("TMP"), true);
    std::cout << "print_tuple(1, 2.5, \"TMP\", true): ";
    print_tuple(t);
}

// =============================================================================
// 4. IF CONSTEXPR — compile-time branch selection (C++17)
// =============================================================================

// Without if constexpr we'd need tag dispatch or specializations
template <typename T>
std::string describe_type(const T& value) {
    if constexpr (std::is_integral_v<T>) {
        return "integral(" + std::to_string(value) + ")";
    } else if constexpr (std::is_floating_point_v<T>) {
        return "float(" + std::to_string(value) + ")";
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "string(\"" + value + "\")";
    } else {
        return "unknown";
    }
}

// if constexpr in recursive variadic function — avoids the base-case overload
template <typename... Args>
void typed_print(Args&&... args) {
    ([](auto&& v) {
        if constexpr (std::is_integral_v<std::remove_cvref_t<decltype(v)>>) {
            std::cout << "[int:" << v << "] ";
        } else if constexpr (std::is_floating_point_v<std::remove_cvref_t<decltype(v)>>) {
            std::cout << "[float:" << v << "] ";
        } else {
            std::cout << "[other:" << v << "] ";
        }
    }(std::forward<Args>(args)), ...);
    std::cout << "\n";
}

void TMPSample::demonstrate_if_constexpr() {
    std::cout << "\n=== if constexpr (C++17) ===\n";

    std::cout << "describe_type(42)       : " << describe_type(42)             << "\n";
    std::cout << "describe_type(3.14)     : " << describe_type(3.14)           << "\n";
    std::cout << "describe_type(string)   : " << describe_type(std::string("hello")) << "\n";
    std::cout << "describe_type(true)     : " << describe_type(true)           << "\n";

    std::cout << "typed_print(1, 2.5, \"hello\", 'A'): ";
    typed_print(1, 2.5, std::string("hello"), 'A');
}

// =============================================================================
// 5. FOLD EXPRESSIONS (C++17) — compact variadic operations
// =============================================================================

// Unary right fold: (args op ...)
template <typename... Args>
auto fold_sum(Args... args) {
    return (... + args);   // left fold: ((a + b) + c) + ...
}

template <typename... Args>
auto fold_product(Args... args) {
    return (args * ...);   // right fold: a * (b * (c * ...))
}

template <typename... Args>
bool fold_all_positive(Args... args) {
    return ((args > 0) && ...);
}

template <typename... Args>
bool fold_any_negative(Args... args) {
    return ((args < 0) || ...);
}

template <typename T, typename... Args>
void push_all(std::vector<T>& vec, Args&&... args) {
    (vec.push_back(std::forward<Args>(args)), ...);
}

void TMPSample::demonstrate_fold_expressions() {
    std::cout << "\n=== Fold Expressions (C++17) ===\n";

    std::cout << "fold_sum(1,2,3,4,5)         = " << fold_sum(1, 2, 3, 4, 5)         << "\n";
    std::cout << "fold_product(1,2,3,4,5)     = " << fold_product(1, 2, 3, 4, 5)     << "\n";
    std::cout << "fold_all_positive(1,2,3)    = " << fold_all_positive(1, 2, 3)      << "\n";
    std::cout << "fold_all_positive(1,-1,3)   = " << fold_all_positive(1, -1, 3)     << "\n";
    std::cout << "fold_any_negative(1,2,3)    = " << fold_any_negative(1, 2, 3)      << "\n";
    std::cout << "fold_any_negative(1,-2,3)   = " << fold_any_negative(1, -2, 3)     << "\n";

    std::vector<int> v;
    push_all(v, 10, 20, 30, 40);
    std::cout << "push_all result: ";
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";
}

// =============================================================================
// 6. TEMPLATE SPECIALIZATION — full & partial
// =============================================================================

// Primary template
template <typename T, typename U = void>
struct TypeDescriptor {
    static std::string describe() { return "generic type"; }
};

// Full specialization for int
template <>
struct TypeDescriptor<int> {
    static std::string describe() { return "32-bit signed integer"; }
};

// Full specialization for double
template <>
struct TypeDescriptor<double> {
    static std::string describe() { return "64-bit IEEE-754 double"; }
};

// Partial specialization: any pointer type
template <typename T>
struct TypeDescriptor<T*> {
    static std::string describe() { return "pointer to " + TypeDescriptor<T>::describe(); }
};

// Partial specialization via SFINAE/enable_if for integral types
template <typename T>
struct TypeDescriptor<T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, int>>> {
    static std::string describe() {
        return std::string(std::is_signed_v<T> ? "signed" : "unsigned")
             + " integral (" + std::to_string(sizeof(T)) + " bytes)";
    }
};

// Policy-based design via template parameters (a TMP classic)
struct AscendingPolicy  { bool operator()(int a, int b) const { return a < b; } };
struct DescendingPolicy { bool operator()(int a, int b) const { return a > b; } };

template <typename ComparePolicy>
class SortedBuffer {
    std::vector<int> data_;
    ComparePolicy    cmp_;
public:
    void insert(int v) {
        auto pos = std::lower_bound(data_.begin(), data_.end(), v, cmp_);
        data_.insert(pos, v);
    }
    void print(std::string_view label) const {
        std::cout << label << ": ";
        for (int x : data_) std::cout << x << " ";
        std::cout << "\n";
    }
};

void TMPSample::demonstrate_template_specialization() {
    std::cout << "\n=== Template Specialization ===\n";

    std::cout << "TypeDescriptor<float>::describe()    : " << TypeDescriptor<float>::describe()    << "\n";
    std::cout << "TypeDescriptor<int>::describe()      : " << TypeDescriptor<int>::describe()      << "\n";
    std::cout << "TypeDescriptor<double>::describe()   : " << TypeDescriptor<double>::describe()   << "\n";
    std::cout << "TypeDescriptor<int*>::describe()     : " << TypeDescriptor<int*>::describe()     << "\n";
    std::cout << "TypeDescriptor<double*>::describe()  : " << TypeDescriptor<double*>::describe()  << "\n";
    std::cout << "TypeDescriptor<int**>::describe()    : " << TypeDescriptor<int**>::describe()    << "\n";
    std::cout << "TypeDescriptor<long>::describe()     : " << TypeDescriptor<long>::describe()     << "\n";
    std::cout << "TypeDescriptor<unsigned char>::desc(): " << TypeDescriptor<unsigned char>::describe() << "\n";

    std::cout << "\nPolicy-based SortedBuffer:\n";
    SortedBuffer<AscendingPolicy>  asc;
    SortedBuffer<DescendingPolicy> desc;
    for (int x : {5, 1, 4, 2, 3}) { asc.insert(x); desc.insert(x); }
    asc.print("Ascending ");
    desc.print("Descending");
}

// =============================================================================
// 7. SFINAE vs. CONCEPTS — evolution of compile-time constraints
// =============================================================================

// Old-style SFINAE: enable a function only for arithmetic types
template <typename T,
          typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T sfinae_square(T x) { return x * x; }

// C++20 Concepts version — much cleaner
template <typename T>
requires std::integral<T> || std::floating_point<T>
T concept_square(T x) { return x * x; }

// Abbreviated concept syntax
auto abbreviated_square(std::integral auto x)       { return x * x; }
auto abbreviated_square_f(std::floating_point auto x) { return x * x; }

// Custom concept
template <typename T>
concept Printable = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream&>;
};

template <Printable T>
void safe_print(const T& v) {
    std::cout << "  " << v << "\n";
}

void TMPSample::demonstrate_sfinae_vs_concepts() {
    std::cout << "\n=== SFINAE vs. Concepts ===\n";

    std::cout << "SFINAE sfinae_square(5)    = " << sfinae_square(5)    << "\n";
    std::cout << "SFINAE sfinae_square(3.14) = " << sfinae_square(3.14) << "\n";

    std::cout << "Concept concept_square(7)   = " << concept_square(7)   << "\n";
    std::cout << "Concept concept_square(2.5) = " << concept_square(2.5) << "\n";

    std::cout << "Abbreviated abbreviated_square(9)    = " << abbreviated_square(9)      << "\n";
    std::cout << "Abbreviated abbreviated_square_f(1.5)= " << abbreviated_square_f(1.5f) << "\n";

    std::cout << "Printable concept:\n";
    safe_print(42);
    safe_print(std::string("template meta programming"));
    safe_print(3.14);
}

// =============================================================================
// 8. C++20 TMP FEATURES
// =============================================================================

// consteval: guaranteed compile-time evaluation
consteval int ce_square(int n) { return n * n; }

// Non-type template parameters (NTTP) of any literal type (C++20)
template <double Value>
struct CompileTimeDouble {
    static constexpr double value = Value;
};

// Template lambda (C++20)
auto gen_adder = []<typename T>(T a, T b) -> T { return a + b; };

// std::is_constant_evaluated() (C++20)
constexpr int adaptive_fn(int n) {
    if (std::is_constant_evaluated()) {
        // Compile-time path
        int result = 1;
        for (int i = 1; i <= n; ++i) result *= i;
        return result;
    } else {
        // Runtime path (could use a lookup table, etc.)
        int result = 1;
        for (int i = 1; i <= n; ++i) result *= i;
        return result;
    }
}

void TMPSample::demonstrate_cpp20_tmp() {
    std::cout << "\n=== C++20 TMP Features ===\n";

    // consteval
    constexpr int v = ce_square(7);
    std::cout << "consteval ce_square(7) = " << v << "\n";
    static_assert(ce_square(7) == 49);

    // NTTP with floating-point (C++20 allows this)
    std::cout << "CompileTimeDouble<3.14>::value = " << CompileTimeDouble<3.14>::value << "\n";

    // Template lambda
    std::cout << "gen_adder<int>(3,4)     = " << gen_adder(3, 4)        << "\n";
    std::cout << "gen_adder<double>(1.5,2.5) = " << gen_adder(1.5, 2.5) << "\n";

    // std::is_constant_evaluated
    constexpr int ct = adaptive_fn(5);
    int           rt = adaptive_fn(5);
    std::cout << "adaptive_fn(5) at compile-time = " << ct << "\n";
    std::cout << "adaptive_fn(5) at runtime      = " << rt << "\n";

    // Comparison of TMP evolution
    std::cout << "\n=== TMP Evolution Table ===\n";
    std::cout << "+----------+--------------------------------------------+\n";
    std::cout << "| C++98/03 | Recursive struct, specialization, SFINAE   |\n";
    std::cout << "| C++11    | variadic templates, constexpr, type_traits  |\n";
    std::cout << "| C++14    | variable templates, constexpr relaxation    |\n";
    std::cout << "| C++17    | if constexpr, fold expressions, CTAD        |\n";
    std::cout << "| C++20    | Concepts, consteval, NTTPs, template lambda |\n";
    std::cout << "+----------+--------------------------------------------+\n";
}

// =============================================================================
// run() — entry point
// =============================================================================

#include "SampleRegistry.hpp"

void TMPSample::run() {
    std::cout << "Running Template Meta Programming (TMP) Sample...\n";

    demonstrate_compile_time_values();
    demonstrate_type_traits();
    demonstrate_variadic_templates();
    demonstrate_if_constexpr();
    demonstrate_fold_expressions();
    demonstrate_template_specialization();
    demonstrate_sfinae_vs_concepts();
    demonstrate_cpp20_tmp();

    std::cout << "\n=== TMP Summary ===\n";
    std::cout << "Template Meta Programming moves computation from runtime to compile time.\n";
    std::cout << "Key techniques:\n";
    std::cout << "  - Recursive struct templates & constexpr for compile-time values\n";
    std::cout << "  - Type traits for type inspection and transformation\n";
    std::cout << "  - Variadic templates for generic N-ary operations\n";
    std::cout << "  - if constexpr for zero-cost compile-time branching\n";
    std::cout << "  - Fold expressions for compact variadic operations\n";
    std::cout << "  - Template specialization for type-specific behavior\n";
    std::cout << "  - SFINAE / Concepts for constraining templates\n";
    std::cout << "  - C++20: consteval, NTTPs, template lambdas\n";
    std::cout << "\nTMP demonstration completed!\n";
}

// Auto-register this sample
REGISTER_SAMPLE(TMPSample, "Template Meta Programming (TMP)", 28);
