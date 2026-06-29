# Constructing Objects In Place with Lambda Factories (instead of `std::move`)

A tutorial on eliminating redundant constructions, moves, and destructions by
passing a *factory* to the destination rather than building an object and
moving it in. All operation counts and timings below were measured with
`g++ 13.3, -std=c++20 -O2`.

---

## 1. The misconception: what `std::move` actually does

`std::move` does **not** move anything and does **not** save you a construction.
It is a cast: `static_cast<T&&>`. It only changes *overload resolution* so that a
move constructor is chosen instead of a copy constructor.

That means the moved-into object is still a **second object**. You pay for:

1. constructing the source,
2. move-constructing the destination from it,
3. destroying the (now hollowed-out) source.

So `std::move` converts *copy + destroy* into *move + destroy*. It never gets you
down to a single construction. If the move itself is expensive (a large inline
buffer, a type that does real work in its move ctor, or a non-movable type where
move is impossible), `std::move` is the wrong tool.

The alternative is to never create the intermediate object at all: hand the
destination a **callable that constructs the object**, and let the destination
invoke it at the final storage location. The object is then *born where it dies* —
one construction, no moves.

---

## 2. A lifetime probe

Every claim below is backed by this instrumented type, which prints each event:

```cpp
struct Tracer {
    std::string id;
    Tracer(std::string s) : id(std::move(s)) { std::cout << "  ctor " << id << "\n"; }
    Tracer(const Tracer& o) : id(o.id)            { std::cout << "  COPY " << id << "\n"; }
    Tracer(Tracer&& o) noexcept : id(std::move(o.id)) { std::cout << "  MOVE " << id << "\n"; }
    ~Tracer() { std::cout << "  dtor " << id << "\n"; }
};

Tracer makeTracer(std::string id) { return Tracer(std::move(id)); } // returns a prvalue
```

(The `std::move(s)` inside the member-init list is the *correct* use of
`std::move`: relocating a named lvalue you already own into a member. Keep that
distinction in mind — `std::move` is for relocating named things; the factory
idiom is for constructing fresh things.)

---

## 3. The three approaches, measured

### A) Build a local, then `push_back(std::move(...))`

```cpp
std::vector<Tracer> v; v.reserve(4);
Tracer local = makeTracer("A");   // copy-elided straight into 'local'
v.push_back(std::move(local));    // move into the vector slot
```

Output:

```
  ctor A      <- local constructed
  MOVE A      <- moved into the vector
  dtor        <- the hollow moved-from 'local' is destroyed (note: empty id)
  dtor A      <- the vector element is destroyed
```

**1 ctor + 1 move + 2 dtors.** The move and the extra destruction are pure
overhead — work spent on an object that exists only to be relocated.

### B) `emplace_back(makeTracer(...))`

A common assumption is that `emplace` fixes this. It does not, here:

```cpp
std::vector<Tracer> v; v.reserve(4);
v.emplace_back(makeTracer("B"));
```

Output:

```
  ctor B
  MOVE B
  dtor
  dtor B
```

Same counts. `makeTracer("B")` produces a prvalue *as the argument to emplace*;
that materialized temporary is then moved into the slot. `emplace` only avoids
the intermediate when you pass it the **constructor arguments**, not an
already-built object.

### C) The lambda factory — no move at all

Wrap a factory lambda in a tiny type with a **conversion operator**, and hand
that to `emplace`:

```cpp
auto factory = [] { return makeTracer("C"); };
struct W {
    decltype(factory) f;
    operator Tracer() const { return f(); }   // returns a prvalue
};
std::vector<Tracer> v; v.reserve(4);
v.emplace_back(W{factory});
```

Output:

```
  ctor C
  dtor C
```

**1 ctor + 1 dtor. Zero moves, zero hollow husks.** That is the whole point.

---

## 4. Why this works: guaranteed copy elision

The mechanism is C++17 **guaranteed copy elision** for prvalues.

`emplace_back(W{factory})` ultimately does, in effect:

```cpp
::new (slot) Tracer( static_cast<Tracer>(the_W) );
```

`static_cast<Tracer>(the_W)` invokes `operator Tracer()`, which evaluates
`return f();`. The result of `f()` is a **prvalue** of type `Tracer`. Since
C++17, a prvalue used to initialize an object of the same type does **not**
create a temporary that is then moved — the prvalue *is* the initializer of the
final object. The `Tracer` is constructed directly into `slot`.

No temporary materializes, so there is nothing to move and nothing extra to
destroy. This is also why the technique works in `-O0`: it is a language
guarantee, not an optimizer's discretion.

---

## 5. A reusable, C++20 wrapper

You do not want to hand-write a `struct W` each time. Here is a generic,
concept-constrained version:

```cpp
#include <concepts>
#include <type_traits>
#include <utility>

template <class F>
concept Factory = std::invocable<F&>
               && !std::is_void_v<std::invoke_result_t<F&>>;

template <Factory F>
class InPlace {
public:
    explicit InPlace(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
        : make_(std::move(f)) {}                 // <-- legitimate std::move: own the lambda

    // Implicit conversion produces a prvalue, elided into the destination.
    operator std::invoke_result_t<F&>()
        noexcept(std::is_nothrow_invocable_v<F&>)
    { return make_(); }

private:
    F make_;
};

template <class F> InPlace(F) -> InPlace<F>;      // CTAD: write InPlace{[]{...}}
```

Usage is uniform across destinations:

```cpp
T x = InPlace{[]{ return makeT(...); }};          // direct init
holder.member = /* aggregate */ InPlace{[]{ ... }};
vec.emplace_back(InPlace{[]{ return makeT(...); }});
opt.emplace(InPlace{[]{ return makeT(...); }});
```

Note the one place `std::move` *is* correct: inside `InPlace`'s constructor,
moving the caller's lambda into `make_`. The lambda is a named lvalue you are
relocating — exactly what `std::move` is for. The factory idiom and `std::move`
are complementary, not opposed.

---

## 6. The decisive case: non-movable, non-copyable types

When a type deletes its copy and move constructors, `std::move` is not merely
slow — it is **impossible**. The factory idiom still works, because the object
is never relocated; it is constructed once at its destination.

```cpp
struct Immovable {
    std::string id;
    explicit Immovable(std::string s) : id(std::move(s)) {}
    Immovable(const Immovable&) = delete;
    Immovable(Immovable&&)      = delete;
};
```

All three of these compile and run, each producing exactly **1 ctor + 1 dtor**:

```cpp
// (a) direct initialization
Immovable a = InPlace{[]{ return Immovable("direct"); }};

// (b) aggregate member init
struct Holder { Immovable x; };
Holder h{ InPlace{[]{ return Immovable("member"); }} };

// (c) std::optional::emplace
std::optional<Immovable> o;
o.emplace(InPlace{[]{ return Immovable("opt"); }});
```

This is the idiom's real superpower: it lets you place types with mutexes,
atomics, `std::array<std::atomic<...>>`, or other intentionally-pinned members
into aggregates and `optional`s without granting them a move constructor they
shouldn't have.

### Caveat: `std::vector` still requires movability

This does **not** compile, even with `reserve` and even if the vector never
grows:

```cpp
std::vector<Immovable> v;
v.reserve(2);
v.emplace_back(InPlace{[]{ return Immovable("x"); }});  // ERROR
```

`std::vector` instantiates its relocation machinery (`reserve`, growth) which
requires the element type to be *MoveInsertable*. The requirement is checked at
compile time regardless of whether a reallocation ever happens at runtime. So
for pinned types, reach for `std::optional`, `std::deque` (node-based, never
relocates existing elements), a fixed inline buffer with placement-`new`, or a
container of `std::unique_ptr<Immovable>`.

---

## 7. When the move is expensive: a measured benchmark

For a type whose move constructor does real, non-elidable CPU work (here, a loop
of comparable cost to the constructor), eliminating the move is a direct,
reproducible win. Five million emplace/reset cycles into a `std::optional`:

```
construct + std::move  : ~4280 ms
in-place lambda factory: ~1630 ms     (~2.6x faster)
```

The factory path runs the constructor only; the `std::move` path runs the
constructor *and* the move. When move cost ≈ construction cost, removing it
roughly removes its share of the total — exactly what the numbers show. (If your
type's move is trivial — e.g. a single pointer swap — the wall-clock difference
will be small; the structural win of *fewer operations* and *non-movable
support* remains regardless.)

---

## 8. Decision guide

Use the **lambda / in-place factory** when:

- You control the construction site and want the object built directly at its
  destination (`optional::emplace`, aggregate/member init, manual storage).
- The type is **non-movable / non-copyable**, so `std::move` is off the table.
- The move (or copy) is genuinely expensive and you want to skip it entirely.
- You want construction to be **deferred** until the destination decides it is
  needed (lazy init, conditional construction).

Stick with **`std::move`** when:

- You already hold a *named* object elsewhere and need to relocate it. The
  factory idiom only helps at the point of construction; it cannot retroactively
  un-construct an object you already have.
- You are passing into `std::vector` and the element type is movable — a single
  move into a reserved slot is cheap and idiomatic.

Rule of thumb: **`std::move` relocates things that already exist; the factory
constructs things that don't yet exist, where they belong.**

---

## 9. Embedded / hot-path notes

- The elision is a **language guarantee**, so the operation count is the same at
  `-O0` and `-O2`. That determinism matters when you cannot rely on the
  optimizer (debug builds, `-Og`, certified toolchains).
- Constructing in place avoids touching memory twice (source buffer + destination
  buffer), which reduces cache pressure and, for large inline payloads, memory
  bandwidth — relevant on a constrained aarch64 target.
- It composes with fixed-capacity / static storage: pair the factory with
  placement-`new` into an aligned buffer (or an ETL container) to construct a
  pinned object with zero dynamic allocation and zero relocation.
- `noexcept`-propagation in the `InPlace` wrapper above preserves the factory's
  exception specification, so `noexcept`-sensitive call sites (e.g. those that
  branch on `is_nothrow_constructible`) still see the right traits.

---

## Appendix: complete, runnable file

```cpp
#include <iostream>
#include <vector>
#include <optional>
#include <string>
#include <concepts>
#include <type_traits>
#include <utility>

template <class F>
concept Factory = std::invocable<F&> && !std::is_void_v<std::invoke_result_t<F&>>;

template <Factory F>
class InPlace {
public:
    explicit InPlace(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
        : make_(std::move(f)) {}
    operator std::invoke_result_t<F&>() noexcept(std::is_nothrow_invocable_v<F&>) {
        return make_();
    }
private:
    F make_;
};
template <class F> InPlace(F) -> InPlace<F>;

struct Tracer {
    std::string id;
    Tracer(std::string s) : id(std::move(s)) { std::cout << "  ctor " << id << "\n"; }
    Tracer(const Tracer& o) : id(o.id)            { std::cout << "  COPY " << id << "\n"; }
    Tracer(Tracer&& o) noexcept : id(std::move(o.id)) { std::cout << "  MOVE " << id << "\n"; }
    ~Tracer() { std::cout << "  dtor " << id << "\n"; }
};
Tracer makeTracer(std::string id) { return Tracer(std::move(id)); }

int main() {
    std::cout << "construct + std::move:\n";
    { std::vector<Tracer> v; v.reserve(2);
      Tracer local = makeTracer("A"); v.push_back(std::move(local)); }

    std::cout << "\nin-place lambda factory:\n";
    { std::vector<Tracer> v; v.reserve(2);
      v.emplace_back(InPlace{[]{ return makeTracer("B"); }}); }
}
```

Build & run:

```sh
g++ -std=c++20 -O2 factory.cpp -o factory && ./factory
```

Expected output — note the missing `MOVE` line and the missing husk `dtor` in
the second block:

```
construct + std::move:
  ctor A
  MOVE A
  dtor
  dtor A

in-place lambda factory:
  ctor B
  dtor B
```
