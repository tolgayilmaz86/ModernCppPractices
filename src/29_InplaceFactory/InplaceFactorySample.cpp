#include "InplaceFactorySample.hpp"
#include <iostream>
#include <optional>
#include <vector>

// Helper class for demonstration: prints every ctor/copy/move/dtor event so
// the operation counts in each demo can be observed directly.
class InplaceFactorySample::Tracer {
public:
    std::string id;

    explicit Tracer(std::string s) : id(std::move(s)) { std::cout << "  ctor " << id << "\n"; }
    Tracer(const Tracer& o) : id(o.id) { std::cout << "  COPY " << id << "\n"; }
    Tracer(Tracer&& o) noexcept : id(std::move(o.id)) { std::cout << "  MOVE " << id << "\n"; }
    ~Tracer() { std::cout << "  dtor " << id << "\n"; }
};

InplaceFactorySample::Tracer InplaceFactorySample::makeTracer(std::string id) {
    return Tracer(std::move(id)); // returns a prvalue
}

void InplaceFactorySample::demonstrate_move_overhead() {
    std::cout << "\n=== A) Build a local, then push_back(std::move(...)) ===\n";
    std::cout << "std::move does not move anything - it is just a cast that picks the\n";
    std::cout << "move constructor. The 'local' object still gets fully constructed,\n";
    std::cout << "then moved into the vector slot, then destroyed (twice total).\n\n";

    std::vector<Tracer> v;
    v.reserve(4);
    Tracer local = makeTracer("A"); // copy-elided straight into 'local'
    v.push_back(std::move(local));  // move into the vector slot

    std::cout << "\nResult: 1 ctor + 1 move + 2 dtors\n";
}

void InplaceFactorySample::demonstrate_emplace_misconception() {
    std::cout << "\n=== B) emplace_back(makeTracer(...)) - a common false fix ===\n";
    std::cout << "Passing an already-built object to emplace does not help: the\n";
    std::cout << "factory call materializes a temporary first, which still has to be\n";
    std::cout << "moved into the slot. emplace only avoids the move when given the\n";
    std::cout << "constructor arguments directly, not a pre-built object.\n\n";

    std::vector<Tracer> v;
    v.reserve(4);
    v.emplace_back(makeTracer("B"));

    std::cout << "\nResult: 1 ctor + 1 move + 2 dtors (same as A)\n";
}

void InplaceFactorySample::demonstrate_lambda_factory() {
    std::cout << "\n=== C) Lambda factory with a conversion operator ===\n";
    std::cout << "Wrap a factory lambda in a tiny type with operator Tracer(). Its\n";
    std::cout << "conversion returns a prvalue, which C++17 guaranteed copy elision\n";
    std::cout << "constructs directly into the destination - no move, no husk.\n\n";

    auto factory = [] { return makeTracer("C"); };
    struct W {
        decltype(factory) f;
        operator Tracer() const { return f(); } // returns a prvalue
    };

    std::vector<Tracer> v;
    v.reserve(4);
    v.emplace_back(W{factory});

    std::cout << "\nResult: 1 ctor + 1 dtor. Zero moves, zero hollow husks.\n";
}

void InplaceFactorySample::demonstrate_inplace_wrapper() {
    std::cout << "\n=== D) The reusable InPlace<F> wrapper ===\n";
    std::cout << "Same trick as C, generalized behind a concept-constrained class\n";
    std::cout << "template so you do not hand-write 'struct W' at every call site.\n\n";

    std::vector<Tracer> v;
    v.reserve(4);
    v.emplace_back(InPlace{[] { return makeTracer("D"); }});

    std::cout << "\nResult: 1 ctor + 1 dtor, uniform across emplace/optional/aggregates.\n";
}

void InplaceFactorySample::demonstrate_immovable_types() {
    std::cout << "\n=== E) Non-movable, non-copyable types ===\n";
    std::cout << "When copy and move are both deleted, std::move is not slow - it is\n";
    std::cout << "impossible. The factory idiom still works because the object is\n";
    std::cout << "never relocated; it is constructed once at its destination.\n\n";

    struct Immovable {
        std::string id;
        explicit Immovable(std::string s) : id(std::move(s)) {
            std::cout << "  ctor " << id << "\n";
        }
        ~Immovable() { std::cout << "  dtor " << id << "\n"; }
        Immovable(const Immovable&) = delete;
        Immovable(Immovable&&) = delete;
    };

    std::cout << "-- direct initialization --\n";
    Immovable a = InPlace{[] { return Immovable("direct"); }};

    std::cout << "-- aggregate member init --\n";
    struct Holder { Immovable x; };
    Holder h{InPlace{[] { return Immovable("member"); }}};

    std::cout << "-- std::optional::emplace --\n";
    std::optional<Immovable> o;
    o.emplace(InPlace{[] { return Immovable("opt"); }});

    std::cout << "\nResult: each of the three sites is 1 ctor + 1 dtor, with a type\n";
    std::cout << "that std::move could never have touched.\n";
}

void InplaceFactorySample::demonstrate_vector_caveat() {
    std::cout << "\n=== F) Caveat: std::vector still requires movability ===\n";
    std::cout << "The InPlace wrapper does NOT make std::vector accept immovable\n";
    std::cout << "types. This would fail to compile even with reserve() and even if\n";
    std::cout << "the vector never grows:\n\n";
    std::cout << "  std::vector<Immovable> v; v.reserve(2);\n";
    std::cout << "  v.emplace_back(InPlace{[]{ return Immovable(\"x\"); }});  // ERROR\n\n";
    std::cout << "std::vector instantiates relocation machinery (reserve/growth) that\n";
    std::cout << "requires the element type to be MoveInsertable, checked at compile\n";
    std::cout << "time regardless of whether a reallocation ever happens at runtime.\n";
    std::cout << "For pinned types, prefer std::optional, std::deque (node-based,\n";
    std::cout << "never relocates existing elements), a fixed inline buffer with\n";
    std::cout << "placement-new, or a container of std::unique_ptr<T>.\n";
}

#include "SampleRegistry.hpp"

void InplaceFactorySample::run() {
    std::cout << "Running In-Place Factory (Lambda Factories vs std::move) Sample...\n";

    demonstrate_move_overhead();
    demonstrate_emplace_misconception();
    demonstrate_lambda_factory();
    demonstrate_inplace_wrapper();
    demonstrate_immovable_types();
    demonstrate_vector_caveat();

    std::cout << "\n=== In-Place Factory Summary ===\n";
    std::cout << "std::move relocates things that already exist (copy+destroy ->\n";
    std::cout << "move+destroy); it never gets you below one construction.\n";
    std::cout << "The lambda/InPlace factory idiom constructs things that don't yet\n";
    std::cout << "exist, directly where they belong, using C++17 guaranteed copy\n";
    std::cout << "elision of prvalues. Use it when:\n";
    std::cout << "  - the type is non-movable/non-copyable,\n";
    std::cout << "  - the move/copy is genuinely expensive,\n";
    std::cout << "  - construction should be deferred to the destination.\n";
    std::cout << "Stick with std::move when relocating a named object you already\n";
    std::cout << "hold, or pushing a movable type into a std::vector.\n";

    std::cout << "\nIn-Place Factory demonstration completed!\n";
}

// Auto-register this sample
REGISTER_SAMPLE(InplaceFactorySample, "In-Place Factory (Lambda Factories vs std::move)", 29);
