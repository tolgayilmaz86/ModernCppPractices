#include "TagDispatchingSample.hpp"
#include <iostream>
#include <type_traits>
#include <vector>
#include <string>

// Tag types for dispatching
struct input_iterator_tag {};
struct random_access_iterator_tag : input_iterator_tag {};

// Example function: tag dispatching for advance()
template <typename Iterator>
void advance_impl(Iterator& it, int n, input_iterator_tag) {
    std::cout << "advance_impl: input_iterator_tag (linear advance)\n";
    while (n-- > 0) ++it;
}

template <typename Iterator>
void advance_impl(Iterator& it, int n, random_access_iterator_tag) {
    std::cout << "advance_impl: random_access_iterator_tag (fast advance)\n";
    it += n;
}

template <typename Iterator>
void advance(Iterator& it, int n) {
    using category = typename std::conditional_t<
        std::is_pointer_v<Iterator>,
        random_access_iterator_tag,
        input_iterator_tag>;
    advance_impl(it, n, category{});
}

// Overload resolution demo
void process(int) { std::cout << "process(int)\n"; }
void process(double) { std::cout << "process(double)\n"; }
void process(const std::string&) { std::cout << "process(string)\n"; }

// Tag dispatching for custom algorithm
struct fast_tag {};
struct slow_tag {};

template <typename T>
void compute(T value, fast_tag) {
    std::cout << "compute(fast_tag): Fast algorithm for value " << value << "\n";
}

template <typename T>
void compute(T value, slow_tag) {
    std::cout << "compute(slow_tag): Slow algorithm for value " << value << "\n";
}

template <typename T>
void compute_dispatch(T value, bool fast) {
    if (fast) {
        compute(value, fast_tag{});
    } else {
        compute(value, slow_tag{});
    }
}

void TagDispatchingSample::demonstrate_tag_dispatching() {
    std::cout << "\n=== Tag Dispatching: Iterator Advance Example ===\n";
    int arr[] = {1, 2, 3, 4, 5};
    int* ptr = arr;
    std::cout << "Advancing pointer (random access):\n";
    advance(ptr, 3);
    std::cout << "Pointer now points to: " << *ptr << "\n";

    std::vector<int> vec = {10, 20, 30, 40, 50};
    auto it = vec.begin();
    std::cout << "Advancing vector iterator (input):\n";
    advance(it, 2);
    std::cout << "Iterator now points to: " << *it << "\n";
}

void TagDispatchingSample::demonstrate_overload_resolution() {
    std::cout << "\n=== Overload Resolution Example ===\n";
    process(42);
    process(3.14);
    process(std::string("hello"));
}

void TagDispatchingSample::demonstrate_best_practices() {
    std::cout << "\n=== Tag Dispatching Best Practices ===\n";
    std::cout << "Use tag dispatching to select optimal algorithms at compile time.\n" <<
                 "Prefer tag dispatching over SFINAE for simple cases.\n" <<
                 "Use standard tags (std::input_iterator_tag, std::random_access_iterator_tag) when possible.\n";
    std::cout << "\n\nCustom algorithm dispatch example:\n";
    compute_dispatch(100, true);
    compute_dispatch(100, false);
}

void TagDispatchingSample::run() {
    std::cout << "Running Tag Dispatching Sample...\n";
    demonstrate_tag_dispatching();
    demonstrate_overload_resolution();
    demonstrate_best_practices();
    std::cout << "\nTag dispatching demonstration completed!\n";
}
