#include "ProjectionsSample.hpp"
#include "SampleRegistry.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include <functional>
#include <numeric>

// ============================================================================
// SAMPLE DATA STRUCTURE
// ============================================================================

struct Employee {
    std::string name;
    int age;
    double salary;
    std::string department;

    // For demonstration of member function pointers
    std::string getName() const { return name; }
    int getAge() const { return age; }
    double getSalary() const { return salary; }
};

std::ostream& operator<<(std::ostream& os, const Employee& e) {
    return os << "{" << e.name << ", " << e.age << ", $" << e.salary << ", " << e.department << "}";
}

// ============================================================================
// THE OLD WAYS (Pre-C++20)
// ============================================================================

namespace OldWays {

// Method 1: Custom comparator function
bool compareByAge(const Employee& a, const Employee& b) {
    return a.age < b.age;
}

// Method 2: Functor class
struct CompareByName {
    bool operator()(const Employee& a, const Employee& b) const {
        return a.name < b.name;
    }
};

// Method 3: Lambda (C++11+)
// Defined inline where needed

void demonstrate(std::vector<Employee> employees) {
    std::cout << "=== Old Ways (Pre-C++20) ===" << std::endl;

    // Sort by age using function pointer
    std::cout << "\n1. Function pointer comparator (sort by age):" << std::endl;
    std::sort(employees.begin(), employees.end(), compareByAge);
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    // Sort by name using functor
    std::cout << "\n2. Functor comparator (sort by name):" << std::endl;
    std::sort(employees.begin(), employees.end(), CompareByName{});
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    // Sort by salary using lambda
    std::cout << "\n3. Lambda comparator (sort by salary descending):" << std::endl;
    std::sort(employees.begin(), employees.end(), 
              [](const Employee& a, const Employee& b) { 
                  return a.salary > b.salary; 
              });
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    // Find max element by age
    std::cout << "\n4. Lambda for max_element (oldest employee):" << std::endl;
    auto oldest = std::max_element(employees.begin(), employees.end(),
                                   [](const Employee& a, const Employee& b) {
                                       return a.age < b.age;
                                   });
    std::cout << "  Oldest: " << *oldest << std::endl;

    std::cout << "\nProblem: Every algorithm needs a custom comparator that:" << std::endl;
    std::cout << "  - Takes two objects" << std::endl;
    std::cout << "  - Extracts the same member from both" << std::endl;
    std::cout << "  - Compares them" << std::endl;
    std::cout << "  This is repetitive and error-prone!" << std::endl;
}

} // namespace OldWays

// ============================================================================
// THE NEW WAY: PROJECTIONS (C++20)
// ============================================================================

namespace NewWay {

void demonstrate(std::vector<Employee> employees) {
    std::cout << "\n=== New Way: Projections (C++20) ===" << std::endl;
    std::cout << "Projections separate 'what to compare' from 'how to compare'" << std::endl;

    // ========================================================================
    // USING MEMBER POINTERS AS PROJECTIONS
    // ========================================================================
    // C++20 allows pointers to data members to be used as callables!
    // std::invoke(&Employee::age, employee) returns employee.age

    std::cout << "\n1. Pointer to data member as projection (sort by age):" << std::endl;
    std::ranges::sort(employees, std::less{}, &Employee::age);
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    std::cout << "\n2. Pointer to data member (sort by name):" << std::endl;
    std::ranges::sort(employees, {}, &Employee::name);  // {} = default comparator (std::less)
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    std::cout << "\n3. Pointer to data member with greater<> (sort by salary desc):" << std::endl;
    std::ranges::sort(employees, std::greater{}, &Employee::salary);
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    // ========================================================================
    // USING MEMBER FUNCTION POINTERS AS PROJECTIONS
    // ========================================================================
    // Pointers to member functions work too!

    std::cout << "\n4. Pointer to member FUNCTION as projection:" << std::endl;
    std::ranges::sort(employees, {}, &Employee::getName);
    std::cout << "  Sorted by getName(): ";
    for (const auto& e : employees) std::cout << e.name << " ";
    std::cout << std::endl;

    // ========================================================================
    // LAMBDA PROJECTIONS
    // ========================================================================
    // For more complex transformations, use lambdas

    std::cout << "\n5. Lambda projection (sort by name length):" << std::endl;
    std::ranges::sort(employees, {}, [](const Employee& e) { 
        return e.name.length(); 
    });
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    std::cout << "\n6. Lambda projection (sort by age + years until retirement):" << std::endl;
    std::ranges::sort(employees, {}, [](const Employee& e) { 
        return 65 - e.age;  // Years until retirement at 65
    });
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    // ========================================================================
    // PROJECTIONS WITH OTHER ALGORITHMS
    // ========================================================================

    std::cout << "\n7. max_element with projection (highest paid):" << std::endl;
    auto highest_paid = std::ranges::max_element(employees, {}, &Employee::salary);
    std::cout << "  Highest paid: " << *highest_paid << std::endl;

    std::cout << "\n8. min_element with projection (youngest):" << std::endl;
    auto youngest = std::ranges::min_element(employees, {}, &Employee::age);
    std::cout << "  Youngest: " << *youngest << std::endl;

    std::cout << "\n9. find with projection (find 'Engineering' dept):" << std::endl;
    auto engineer = std::ranges::find(employees, "Engineering", &Employee::department);
    if (engineer != employees.end()) {
        std::cout << "  Found: " << *engineer << std::endl;
    }

    std::cout << "\n10. count_if with projection (count age > 30):" << std::endl;
    auto count = std::ranges::count_if(employees, [](int age) { return age > 30; }, 
                                       &Employee::age);
    std::cout << "  Employees over 30: " << count << std::endl;

    // ========================================================================
    // PROJECTIONS WITH VIEWS
    // ========================================================================

    std::cout << "\n11. transform view with projection (extract names):" << std::endl;
    std::cout << "  Names: ";
    for (const auto& name : employees | std::views::transform(&Employee::name)) {
        std::cout << name << " ";
    }
    std::cout << std::endl;

    std::cout << "\n12. filter + transform with projections:" << std::endl;
    std::cout << "  High earners (>70k): ";
    for (const auto& name : employees 
            | std::views::filter([](const Employee& e) { return e.salary > 70000; })
            | std::views::transform(&Employee::name)) {
        std::cout << name << " ";
    }
    std::cout << std::endl;

    // ========================================================================
    // CHAINED/COMPOSED PROJECTIONS
    // ========================================================================

    std::cout << "\n13. Composed projection (sort by first char of name):" << std::endl;
    std::ranges::sort(employees, {}, [](const Employee& e) { 
        return std::tolower(static_cast<unsigned char>(e.name[0])); 
    });
    for (const auto& e : employees) std::cout << "  " << e << std::endl;

    // ========================================================================
    // BINARY ALGORITHMS WITH PROJECTIONS
    // ========================================================================

    std::cout << "\n14. equal with projections (compare by department only):" << std::endl;
    std::vector<Employee> team1 = {{"A", 25, 50000, "Engineering"}, {"B", 30, 60000, "Engineering"}};
    std::vector<Employee> team2 = {{"X", 35, 70000, "Engineering"}, {"Y", 40, 80000, "Engineering"}};
    bool same_dept = std::ranges::equal(team1, team2, {}, &Employee::department, &Employee::department);
    std::cout << "  Teams in same departments: " << std::boolalpha << same_dept << std::endl;

    // ========================================================================
    // UNIQUE WITH PROJECTION
    // ========================================================================

    std::cout << "\n15. unique with projection (unique departments):" << std::endl;
    std::ranges::sort(employees, {}, &Employee::department);
    auto [first, last] = std::ranges::unique(employees, {}, &Employee::department);
    std::cout << "  Unique departments: ";
    for (auto it = employees.begin(); it != first; ++it) {
        std::cout << it->department << " ";
    }
    std::cout << std::endl;
}

} // namespace NewWay

// ============================================================================
// HOW PROJECTIONS WORK UNDER THE HOOD
// ============================================================================

namespace Internals {

void demonstrate() {
    std::cout << "\n=== How Projections Work (std::invoke) ===" << std::endl;
    
    Employee emp{"Alice", 30, 75000, "Engineering"};
    
    // std::invoke unifies calling conventions:
    std::cout << "\nstd::invoke with different callables:" << std::endl;
    
    // 1. Pointer to data member
    std::cout << "  std::invoke(&Employee::name, emp) = " 
              << std::invoke(&Employee::name, emp) << std::endl;
    
    // 2. Pointer to member function
    std::cout << "  std::invoke(&Employee::getName, emp) = " 
              << std::invoke(&Employee::getName, emp) << std::endl;
    
    // 3. Lambda
    auto getAge = [](const Employee& e) { return e.age; };
    std::cout << "  std::invoke(lambda, emp) = " 
              << std::invoke(getAge, emp) << std::endl;
    
    // 4. Function object
    struct GetSalary { double operator()(const Employee& e) const { return e.salary; } };
    std::cout << "  std::invoke(functor, emp) = " 
              << std::invoke(GetSalary{}, emp) << std::endl;
    
    std::cout << "\nThis is why projections can accept:" << std::endl;
    std::cout << "  - Pointers to data members (&Employee::age)" << std::endl;
    std::cout << "  - Pointers to member functions (&Employee::getAge)" << std::endl;
    std::cout << "  - Lambdas ([](const auto& e) { return e.age; })" << std::endl;
    std::cout << "  - Function objects (functors)" << std::endl;
    std::cout << "  - Regular functions" << std::endl;
}

} // namespace Internals

// ============================================================================
// MAIN RUN FUNCTION
// ============================================================================

void ProjectionsSample::run() {
    std::vector<Employee> employees = {
        {"Alice",   30, 75000, "Engineering"},
        {"Bob",     45, 90000, "Management"},
        {"Charlie", 28, 65000, "Engineering"},
        {"Diana",   35, 82000, "Marketing"},
        {"Eve",     32, 71000, "Engineering"},
    };

    OldWays::demonstrate(employees);
    NewWay::demonstrate(employees);
    Internals::demonstrate();

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "+------------------+----------------------------------------+" << std::endl;
    std::cout << "| Old Way          | New Way (Projections)                  |" << std::endl;
    std::cout << "+------------------+----------------------------------------+" << std::endl;
    std::cout << "| sort(b,e,cmp)    | ranges::sort(r, cmp, projection)       |" << std::endl;
    std::cout << "| Custom comparator| Member pointer: &Type::member          |" << std::endl;
    std::cout << "| for each algo    | Reusable across all algorithms         |" << std::endl;
    std::cout << "+------------------+----------------------------------------+" << std::endl;
    std::cout << "\nKey benefits of projections:" << std::endl;
    std::cout << "  1. Separate 'what to extract' from 'how to compare'" << std::endl;
    std::cout << "  2. Member pointers work as callables (via std::invoke)" << std::endl;
    std::cout << "  3. Cleaner, more readable code" << std::endl;
    std::cout << "  4. Less repetition - same projection for different algorithms" << std::endl;
    std::cout << "  5. Default comparator ({}) often sufficient" << std::endl;
}

// Register this sample
REGISTER_SAMPLE(ProjectionsSample, "Projections (C++20 Ranges)", 25);
