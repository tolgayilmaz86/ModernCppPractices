# C++20 Projections

## Overview

**Projections** are a C++20 feature in the Ranges library that allow you to specify *what part of an element* to use in an algorithm, separately from *how to compare* elements. This is a powerful abstraction that simplifies code and reduces repetition.

## The Problem: Repetitive Comparators

Before C++20, sorting or searching by a specific member required writing custom comparators for every algorithm:

```cpp
struct Employee {
    std::string name;
    int age;
    double salary;
};

// Pre-C++20: Need custom comparators everywhere
std::sort(employees.begin(), employees.end(), 
    [](const Employee& a, const Employee& b) { 
        return a.age < b.age;  // Extract age from BOTH, then compare
    });

std::max_element(employees.begin(), employees.end(),
    [](const Employee& a, const Employee& b) { 
        return a.age < b.age;  // Same pattern repeated!
    });
```

**Problems with this approach:**
1. Comparator must extract the member from **both** objects
2. Repetitive pattern: extract, extract, compare
3. Easy to make mistakes (comparing different members)
4. Hard to reuse across algorithms

## The Solution: Projections

Projections separate **what to extract** from **how to compare**:

```cpp
// C++20: Projection specifies what to extract
std::ranges::sort(employees, std::less{}, &Employee::age);
//                           ^^^^^^^^^^   ^^^^^^^^^^^^^^^
//                           comparator   projection (what to extract)

std::ranges::max_element(employees, std::less{}, &Employee::age);
//                                               Same projection, reusable!
```

## Pointers to Members as Callables

A key C++20 feature is that **pointers to data members** and **pointers to member functions** can be used as callables via `std::invoke`:

```cpp
Employee emp{"Alice", 30, 75000.0};

// Pointer to data member
std::invoke(&Employee::age, emp);      // Returns: 30

// Pointer to member function
std::invoke(&Employee::getName, emp);  // Returns: "Alice"
```

This is why you can write:
```cpp
std::ranges::sort(employees, {}, &Employee::age);     // Data member
std::ranges::sort(employees, {}, &Employee::getName); // Member function
```

## Projection Types

### 1. Pointer to Data Member

```cpp
// Sort by age (ascending)
std::ranges::sort(employees, {}, &Employee::age);

// Sort by name
std::ranges::sort(employees, {}, &Employee::name);

// Sort by salary (descending)
std::ranges::sort(employees, std::greater{}, &Employee::salary);
```

### 2. Pointer to Member Function

```cpp
struct Employee {
    std::string getName() const { return name; }
    int getAge() const { return age; }
};

// Use getter as projection
std::ranges::sort(employees, {}, &Employee::getName);
std::ranges::sort(employees, {}, &Employee::getAge);
```

### 3. Lambda Projection

For more complex transformations:

```cpp
// Sort by name length
std::ranges::sort(employees, {}, [](const Employee& e) { 
    return e.name.length(); 
});

// Sort by computed value
std::ranges::sort(employees, {}, [](const Employee& e) { 
    return e.salary / e.age;  // Salary per year of age
});
```

### 4. `std::identity` (Default)

When no projection is specified, `std::identity{}` is used (returns the element unchanged):

```cpp
// These are equivalent:
std::ranges::sort(employees, std::less{}, std::identity{});
std::ranges::sort(employees, std::less{});
std::ranges::sort(employees);  // Uses default comparator too
```

## Common Algorithms with Projections

### Sorting

```cpp
std::ranges::sort(employees, {}, &Employee::age);
std::ranges::stable_sort(employees, {}, &Employee::name);
std::ranges::partial_sort(employees, employees.begin() + 3, {}, &Employee::salary);
```

### Searching

```cpp
// Find employee named "Alice"
auto it = std::ranges::find(employees, "Alice", &Employee::name);

// Find in sorted range
std::ranges::lower_bound(employees, 30, {}, &Employee::age);
std::ranges::binary_search(employees, 30, {}, &Employee::age);
```

### Min/Max

```cpp
auto oldest = std::ranges::max_element(employees, {}, &Employee::age);
auto youngest = std::ranges::min_element(employees, {}, &Employee::age);
auto [youngest, oldest] = std::ranges::minmax_element(employees, {}, &Employee::age);
```

### Counting

```cpp
// Count employees in Engineering
auto count = std::ranges::count(employees, "Engineering", &Employee::department);

// Count employees over 30
auto count = std::ranges::count_if(employees, 
    [](int age) { return age > 30; },  // Predicate receives projected value!
    &Employee::age);
```

### Uniqueness

```cpp
// Remove consecutive duplicates by department
std::ranges::sort(employees, {}, &Employee::department);
// Now employees are sorted by department
auto [first, last] = std::ranges::unique(employees, {}, &Employee::department);
// Now [first, last) contains unique departments
```

### Comparison

```cpp
// Check if two ranges have same departments
bool same = std::ranges::equal(team1, team2, {}, 
    &Employee::department,   // Projection for first range
    &Employee::department);  // Projection for second range
```

## Projections with Views

Projections work beautifully with range views:

```cpp
// Extract all names
// read this as; for each employee in employees, project the name member
// std::transform will apply the projection to each element
for (auto& name : employees | std::views::transform(&Employee::name)) {
    std::cout << name << "\n";
}

// Filter and project
// Filter employees with salary > 70000, then project names
auto high_earner_names = employees 
    | std::views::filter([](const Employee& e) { return e.salary > 70000; })
    | std::views::transform(&Employee::name);
```

## Old Way vs New Way Comparison

| Task | Old Way (Pre-C++20) | New Way (C++20 Projections) |
|------|---------------------|----------------------------|
| Sort by age | `sort(b, e, [](auto& a, auto& b) { return a.age < b.age; })` | `ranges::sort(r, {}, &T::age)` |
| Find by name | `find_if(b, e, [&](auto& x) { return x.name == target; })` | `ranges::find(r, target, &T::name)` |
| Max by salary | `max_element(b, e, [](auto& a, auto& b) { return a.salary < b.salary; })` | `ranges::max_element(r, {}, &T::salary)` |
| Count dept | `count_if(b, e, [](auto& x) { return x.dept == "Eng"; })` | `ranges::count(r, "Eng", &T::dept)` |

## How It Works: `std::invoke`

Projections work because C++20 algorithms use `std::invoke` internally:

```cpp
// Simplified algorithm implementation
template<typename Range, typename Comp, typename Proj>
void sort(Range&& r, Comp comp = {}, Proj proj = {}) {
    // For each comparison:
    auto projected_a = std::invoke(proj, a);  // Apply projection
    auto projected_b = std::invoke(proj, b);
    if (std::invoke(comp, projected_a, projected_b)) {
        // a comes before b
    }
}
```

`std::invoke` handles all callable types uniformly:
- Regular functions: `std::invoke(func, arg)` → `func(arg)`
- Member functions: `std::invoke(&T::func, obj)` → `obj.func()`
- Data members: `std::invoke(&T::member, obj)` → `obj.member`
- Lambdas/functors: `std::invoke(callable, arg)` → `callable(arg)`

## Best Practices

1. **Use member pointers** for simple extractions - cleaner than lambdas
e.g.: `&Employee::age` instead of `[](const Employee& e) { return e.age; }`
2. **Use lambdas** for computed/transformed values
e.g.: sorting by name length or derived values
3. **Use `{}`** for default comparator when projection is sufficient
4. **Prefer `std::ranges::` algorithms** over `std::` for projection support
5. **Remember**: Predicate in `count_if`, `find_if`, etc. receives the **projected value**

## Expected Output

```
=== Old Ways (Pre-C++20) ===
1. Function pointer comparator (sort by age): ...
2. Functor comparator (sort by name): ...
3. Lambda comparator (sort by salary descending): ...

=== New Way: Projections (C++20) ===
1. Pointer to data member as projection (sort by age): ...
2. Pointer to data member (sort by name): ...
3. Pointer to member FUNCTION as projection: ...
...
```

## Related Topics

- [24_ThreeWayComparison](../24_ThreeWayComparison/README.md) - Custom comparisons with `<=>`
- [16_Concepts](../16_Concepts/README.md) - Constraining projection types

## References

- [cppreference: Ranges algorithms](https://en.cppreference.com/w/cpp/algorithm/ranges)
- [cppreference: std::invoke](https://en.cppreference.com/w/cpp/utility/functional/invoke)
- [P0896R4 - The One Ranges Proposal](https://wg21.link/p0896r4)
