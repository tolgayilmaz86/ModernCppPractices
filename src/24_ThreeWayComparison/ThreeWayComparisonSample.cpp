#include "ThreeWayComparisonSample.hpp"
#include "SampleRegistry.hpp"
#include <iostream>
#include <compare>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

// ============================================================================
// STRONG ORDERING EXAMPLE
// ============================================================================
// Strong ordering: a == b means they are identical/substitutable
// Provides: ==, !=, <, >, <=, >=

class Person {
public:
    Person(std::string name, int age) : name_(std::move(name)), age_(age) {}

    // Defaulted <=> generates all comparison operators automatically
    // For members that support <=>, this is often sufficient
    auto operator<=>(const Person&) const = default;
    
    // Defaulted == is automatically generated when <=> is defaulted
    // but you can explicitly default it for clarity
    bool operator==(const Person&) const = default;

    std::string getName() const { return name_; }
    int getAge() const { return age_; }

private:
    std::string name_;
    int age_;
};

// ============================================================================
// STRONG ORDERING WITH CUSTOM LOGIC
// ============================================================================
// When you need custom comparison logic but still want strong ordering

class Version {
public:
    Version(int major, int minor, int patch)
        : major_(major), minor_(minor), patch_(patch) {}

    // Custom three-way comparison returning strong_ordering
    std::strong_ordering operator<=>(const Version& other) const {
        // Compare major first
        if (auto cmp = major_ <=> other.major_; cmp != 0) return cmp;
        // Then minor
        if (auto cmp = minor_ <=> other.minor_; cmp != 0) return cmp;
        // Finally patch
        return patch_ <=> other.patch_;
    }

    // When you define custom <=>, you must also define ==
    bool operator==(const Version& other) const {
        return major_ == other.major_ && 
               minor_ == other.minor_ && 
               patch_ == other.patch_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Version& v) {
        return os << v.major_ << "." << v.minor_ << "." << v.patch_;
    }

private:
    int major_, minor_, patch_;
};

// ============================================================================
// WEAK ORDERING EXAMPLE
// ============================================================================
// Weak ordering: equivalent objects may not be identical
// Provides: ==, !=, <, >, <=, >=
// Example: case-insensitive string comparison

class CaseInsensitiveString {
public:
    CaseInsensitiveString(std::string s) : str_(std::move(s)) {}

    std::weak_ordering operator<=>(const CaseInsensitiveString& other) const {
        // Compare character by character, ignoring case
        auto it1 = str_.begin();
        auto it2 = other.str_.begin();
        
        while (it1 != str_.end() && it2 != other.str_.end()) {
            char c1 = std::tolower(static_cast<unsigned char>(*it1));
            char c2 = std::tolower(static_cast<unsigned char>(*it2));
            
            if (c1 < c2) return std::weak_ordering::less;
            if (c1 > c2) return std::weak_ordering::greater;
            ++it1;
            ++it2;
        }
        
        if (str_.size() < other.str_.size()) return std::weak_ordering::less;
        if (str_.size() > other.str_.size()) return std::weak_ordering::greater;
        return std::weak_ordering::equivalent;  // Note: equivalent, not equal!
    }

    bool operator==(const CaseInsensitiveString& other) const {
        return (*this <=> other) == 0;
    }

    const std::string& str() const { return str_; }

private:
    std::string str_;
};

// ============================================================================
// PARTIAL ORDERING EXAMPLE
// ============================================================================
// Partial ordering: some values may be unordered (incomparable)
// Provides: <, > (but NOT <= or >= for unordered values)
// Classic example: floating point with NaN

class OptionalValue {
public:
    OptionalValue() : has_value_(false), value_(0.0) {}
    OptionalValue(double v) : has_value_(true), value_(v) {}

    std::partial_ordering operator<=>(const OptionalValue& other) const {
        // If either doesn't have a value, they are unordered
        if (!has_value_ || !other.has_value_) {
            return std::partial_ordering::unordered;
        }
        
        // Check for NaN (NaN is unordered with everything, including itself)
        if (std::isnan(value_) || std::isnan(other.value_)) {
            return std::partial_ordering::unordered;
        }
        
        // Normal comparison
        if (value_ < other.value_) return std::partial_ordering::less;
        if (value_ > other.value_) return std::partial_ordering::greater;
        return std::partial_ordering::equivalent;
    }

    bool operator==(const OptionalValue& other) const {
        if (!has_value_ || !other.has_value_) return false;
        if (std::isnan(value_) || std::isnan(other.value_)) return false;
        return value_ == other.value_;
    }

    bool hasValue() const { return has_value_; }
    double value() const { return value_; }

private:
    bool has_value_;
    double value_;
};

// ============================================================================
// COMPARISON CATEGORIES SUMMARY
// ============================================================================
/*
 * std::strong_ordering:
 *   - Values: less, equal, greater
 *   - equal means substitutable (identical for all purposes)
 *   - Generates: ==, !=, <, >, <=, >=
 *   - Example: int, pointer comparison
 *
 * std::weak_ordering:
 *   - Values: less, equivalent, greater
 *   - equivalent means same in comparison but may differ otherwise
 *   - Generates: ==, !=, <, >, <=, >=
 *   - Example: case-insensitive strings ("Hello" equivalent to "HELLO")
 *
 * std::partial_ordering:
 *   - Values: less, equivalent, greater, unordered
 *   - Some values cannot be compared
 *   - Generates: ==, !=, <, >
 *   - Example: floating-point with NaN, sets with subset relation
 */

void ThreeWayComparisonSample::run() {
    std::cout << "\n=== Strong Ordering: Person (defaulted <=>) ===" << std::endl;
    {
        Person alice("Alice", 30);
        Person bob("Bob", 25);
        Person alice2("Alice", 30);

        std::cout << "Alice vs Bob:" << std::endl;
        std::cout << "  alice < bob:  " << std::boolalpha << (alice < bob) << std::endl;
        std::cout << "  alice > bob:  " << (alice > bob) << std::endl;
        std::cout << "  alice == bob: " << (alice == bob) << std::endl;
        std::cout << "  alice != bob: " << (alice != bob) << std::endl;

        std::cout << "\nAlice vs Alice2 (same values):" << std::endl;
        std::cout << "  alice == alice2: " << (alice == alice2) << std::endl;
        std::cout << "  alice <= alice2: " << (alice <= alice2) << std::endl;
        std::cout << "  alice >= alice2: " << (alice >= alice2) << std::endl;

        // Sorting with <=>
        std::vector<Person> people = {
            {"Charlie", 35}, {"Alice", 30}, {"Bob", 25}, {"Alice", 20}
        };
        std::sort(people.begin(), people.end());
        std::cout << "\nSorted people: ";
        for (const auto& p : people) {
            std::cout << p.getName() << "(" << p.getAge() << ") ";
        }
        std::cout << std::endl;
    }

    std::cout << "\n=== Strong Ordering: Version (custom <=>) ===" << std::endl;
    {
        Version v1{1, 0, 0};
        Version v2{1, 2, 0};
        Version v3{1, 2, 3};
        Version v4{2, 0, 0};

        std::cout << "v1 = " << v1 << ", v2 = " << v2 << ", v3 = " << v3 << ", v4 = " << v4 << std::endl;
        std::cout << "v1 < v2:  " << (v1 < v2) << std::endl;
        std::cout << "v2 < v3:  " << (v2 < v3) << std::endl;
        std::cout << "v3 < v4:  " << (v3 < v4) << std::endl;
        std::cout << "v1 == v1: " << (v1 == v1) << std::endl;

        // Using <=> directly
        auto cmp = v1 <=> v4;
        if (cmp < 0) std::cout << v1 << " is less than " << v4 << std::endl;
        else if (cmp > 0) std::cout << v1 << " is greater than " << v4 << std::endl;
        else std::cout << v1 << " equals " << v4 << std::endl;
    }

    std::cout << "\n=== Weak Ordering: CaseInsensitiveString ===" << std::endl;
    {
        CaseInsensitiveString s1("Hello");
        CaseInsensitiveString s2("HELLO");
        CaseInsensitiveString s3("World");

        std::cout << "Comparing \"Hello\" and \"HELLO\":" << std::endl;
        std::cout << "  s1 == s2 (equivalent): " << (s1 == s2) << std::endl;
        std::cout << "  s1 < s2:  " << (s1 < s2) << std::endl;
        std::cout << "  s1 <= s2: " << (s1 <= s2) << std::endl;
        
        std::cout << "\nComparing \"Hello\" and \"World\":" << std::endl;
        std::cout << "  s1 < s3:  " << (s1 < s3) << std::endl;
        std::cout << "  s1 > s3:  " << (s1 > s3) << std::endl;

        // Note: strings are equivalent but not identical
        std::cout << "\nNote: \"" << s1.str() << "\" and \"" << s2.str() 
                  << "\" are equivalent but have different representations" << std::endl;
    }

    std::cout << "\n=== Partial Ordering: OptionalValue (with NaN) ===" << std::endl;
    {
        OptionalValue a(5.0);
        OptionalValue b(10.0);
        OptionalValue empty;
        OptionalValue nan_val(std::nan(""));

        std::cout << "Comparing normal values (5.0 vs 10.0):" << std::endl;
        std::cout << "  a < b:  " << (a < b) << std::endl;
        std::cout << "  a > b:  " << (a > b) << std::endl;
        std::cout << "  a == b: " << (a == b) << std::endl;

        std::cout << "\nComparing with empty value (unordered):" << std::endl;
        std::cout << "  a < empty:  " << (a < empty) << std::endl;
        std::cout << "  a > empty:  " << (a > empty) << std::endl;
        std::cout << "  a == empty: " << (a == empty) << std::endl;
        // Note: for unordered values, both < and > return false!

        std::cout << "\nComparing with NaN (unordered):" << std::endl;
        std::cout << "  a < nan:  " << (a < nan_val) << std::endl;
        std::cout << "  a > nan:  " << (a > nan_val) << std::endl;
        std::cout << "  a == nan: " << (a == nan_val) << std::endl;
        std::cout << "  nan == nan: " << (nan_val == nan_val) << std::endl;

        // Demonstrating partial_ordering::unordered
        auto cmp = a <=> empty;
        if (cmp == std::partial_ordering::unordered) {
            std::cout << "\n5.0 and empty are UNORDERED (cannot be compared)" << std::endl;
        }
    }

    std::cout << "\n=== Comparison Categories Hierarchy ===" << std::endl;
    {
        std::cout << "strong_ordering -> weak_ordering -> partial_ordering" << std::endl;
        std::cout << "\nConversion is implicit from stronger to weaker:" << std::endl;
        
        std::strong_ordering strong = std::strong_ordering::less;
        std::weak_ordering weak = strong;      // OK: strong converts to weak
        std::partial_ordering partial = weak;  // OK: weak converts to partial
        
        std::cout << "  strong_ordering::less converts to weak_ordering::less: "
                  << (weak == std::weak_ordering::less) << std::endl;
        std::cout << "  weak_ordering::less converts to partial_ordering::less: "
                  << (partial == std::partial_ordering::less) << std::endl;
    }

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "+--------------------+------------+---------------------------+" << std::endl;
    std::cout << "| Ordering Type      | Values     | Generated Operators       |" << std::endl;
    std::cout << "+--------------------+------------+---------------------------+" << std::endl;
    std::cout << "| strong_ordering    | less       | ==, !=, <, >, <=, >=      |" << std::endl;
    std::cout << "|                    | equal      | (substitutable equality)  |" << std::endl;
    std::cout << "|                    | greater    |                           |" << std::endl;
    std::cout << "+--------------------+------------+---------------------------+" << std::endl;
    std::cout << "| weak_ordering      | less       | ==, !=, <, >, <=, >=      |" << std::endl;
    std::cout << "|                    | equivalent | (equivalent, not equal)   |" << std::endl;
    std::cout << "|                    | greater    |                           |" << std::endl;
    std::cout << "+--------------------+------------+---------------------------+" << std::endl;
    std::cout << "| partial_ordering   | less       | ==, !=, <, >              |" << std::endl;
    std::cout << "|                    | equivalent | (some values unordered)   |" << std::endl;
    std::cout << "|                    | greater    |                           |" << std::endl;
    std::cout << "|                    | unordered  |                           |" << std::endl;
    std::cout << "+--------------------+------------+---------------------------+" << std::endl;
}

// Register this sample
REGISTER_SAMPLE(ThreeWayComparisonSample, "Three-Way Comparison (Spaceship Operator)", 24);
