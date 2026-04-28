# No Fear Templates
### A Guide to C++ Templates & Their Evolution

---

> *"The compiler is your ally. Templates are the language it speaks fluently."*

C++ templates are one of the most powerful features in the language — and also one of the most misunderstood. This guide walks through every major template concept, from classic function templates to C++20 innovations, using real world usage examples such as; math libraries, ECS architectures, asset systems, render pipelines, embedded systems, and more.

---

## Table of Contents

- [Section 1 — Function Templates](#section-1--function-templates)
- [Section 2 — Class Templates](#section-2--class-templates)
- [Section 3 — Passing Arguments](#section-3--passing-arguments)
- [Section 4 — Template Changes in C++11](#section-4--template-changes-in-c11)
- [Section 5 — Template Changes in C++14 & C++17](#section-5--template-changes-in-c14--c17)
- [Section 6 — Template Changes in C++20](#section-6--template-changes-in-c20)

---

# Section 1 — Function Templates

## 1.1 · Function Templates

As functions are being the main building block of any software for templates they are as well. A function template defines a family of functions. The compiler generates a concrete version — an *instantiation* — for each unique combination of arguments it encounters.

**Some context:** A math library needs `lerp` (linear interpolation) for `float`, `double`, `Vec2`, `Vec3`, `Vec4`. Without templates you'd write four copies. With templates, you write one.

```cpp
// Works for any type T that supports +, -, and scalar multiplication
template <typename T>
T lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}

// Usage
float   f  = lerp(0.0f, 1.0f, 0.5f);      // T = float
Vec3    v  = lerp(startPos, endPos, 0.3f); // T = Vec3
Color   c  = lerp(colorA, colorB, alpha);  // T = Color
```

---

## 1.2 · Template Argument Deduction & Instantiation

The compiler deduces template arguments from the function call arguments. You rarely need to spell them out.

```cpp
template <typename T>
T clamp(T value, T lo, T hi) {
    return value < lo ? lo : (value > hi ? hi : value);
}

float health = clamp(105.0f, 0.0f, 100.0f); // T deduced as float
int   ammo   = clamp(ammoCount, 0, maxAmmo); // T deduced as int
```

Each unique `T` causes the compiler to **instantiate** a new version of the function — this is called *implicit instantiation* and happens on demand.

---

## 1.3 · Explicit Template Arguments

Sometimes deduction is ambiguous or you want to force a specific type.

```cpp
template <typename To, typename From>
To narrow_cast(From value) {
    // Debug-only bounds check
    assert(static_cast<From>(static_cast<To>(value)) == value);
    return static_cast<To>(value);
}

// Must specify To — it cannot be deduced from the call site
uint8_t byteVal = narrow_cast<uint8_t>(255);
int16_t shortId = narrow_cast<int16_t>(entityId);
```

---

## 1.4 · Default Arguments

Template parameters can have defaults, just like function parameters.

```cpp
// Default precision is float; engine users can opt into double for physics sims
template <typename Scalar = float, int Dims = 3>
struct Vec {
    Scalar data[Dims]{};
};

Vec<>          v3f;  // Vec<float, 3>  — the everyday vector
Vec<double, 3> v3d;  // double precision for physics integration
Vec<float, 2>  v2f;  // 2D UV coordinate
```

```cpp
template<typename T = float>
T DegreesToRadians(T degrees) {
    return degrees * static_cast<T>(0.01745329251994329577);
}
```

Great for engine utility APIs where `float` is common but `double` is occasionally required.

---

## 1.5 · Argument Conversions

Template deduction is strict — it does **not** apply implicit conversions. This prevents silent precision loss in math-heavy code.

```cpp
template <typename T>
T dot(const T* a, const T* b, int n) { /* ... */ }

float fa[3] = {1,2,3};
double db[3] = {1,2,3};

dot(fa, fa, 3);            // OK — T = float
dot(db, db, 3);            // OK — T = double
// dot(fa, db, 3);         // ERROR — T cannot be both float and double

// Fix: be explicit
dot<double>(fa, db, 3);    // explicit conversion
```

---

## 1.6 · Templates With Specifiers

Function templates work with `inline`, `constexpr`, `static`, `[[nodiscard]]`, and `noexcept` just like normal functions.

> But be careful with `static`; each instantiation gets its own copy of the static variable. To avoid confusion, it's often better to use `static` variables inside a class template or namespace scope instead. 

> `inline` is often used with function templates to allow definitions in headers without violating the One Definition Rule (ODR). `constexpr` enables compile-time evaluation, which is great for math utilities.

```cpp
// Compile-time math used in shader constant buffer layout calculations
template <typename T>
[[nodiscard]] constexpr T align_up(T value, T alignment) noexcept {
    return (value + alignment - 1) & ~(alignment - 1);
}

// Evaluated entirely at compile time when inputs are constexpr
constexpr size_t cbufferSize = align_up(sizeof(PerFrameData), size_t{256});
static_assert(cbufferSize % 256 == 0, "Constant buffer must be 256-byte aligned");
```

---

## 1.7 · Non-Type Template Parameters

Template parameters don't have to be types — they can be compile-time values: 
- integers
- enums
- pointers to linkage
- (since C++20) floats.

```cpp
// Fixed-size ring buffer: size is a compile-time constant, no heap allocation
template <typename T, size_t Capacity>
class RingBuffer {
    T     buf_[Capacity];
    size_t head_ = 0, tail_ = 0, count_ = 0;
public:
    bool push(const T& v) {
        if (count_ == Capacity) return false;
        buf_[tail_] = v;
        tail_ = (tail_ + 1) % Capacity;
        ++count_;
        return true;
    }
    T pop() {
        T v = buf_[head_];
        head_ = (head_ + 1) % Capacity;
        --count_;
        return v;
    }
    bool empty() const { return count_ == 0; }
};

RingBuffer<RenderCommand, 1024> commandQueue;
RingBuffer<AudioEvent, 256>     audioEvents;
```
**Enum example:**
```cpp
// Compile-time state machine for a game entity
enum class State { Idle, Moving, Attacking };
template <State S>
struct StateBehavior;
template <>
struct StateBehavior<State::Idle> {
    static void update(Entity& e) { /* idle behavior */ }
};
template <>
struct StateBehavior<State::Moving> {
    static void update(Entity& e) { /* moving behavior */ }
};
template <>
struct StateBehavior<State::Attacking> {
    static void update(Entity& e) { /* attacking behavior */ }
};
// Usage
template <State S>
void updateEntity(Entity& e) {
    StateBehavior<S>::update(e);
}
```
---

## 1.8 · Overloading Function Templates

A function template can coexist with other templates and non-template overloads. The compiler picks the best match using normal overload resolution — with templates as a fallback.

```cpp
// Generic serialization
template <typename T>
void serialize(std::ostream& out, const T& value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

// Overload for std::string — plain memcpy won't work
void serialize(std::ostream& out, const std::string& value) {
    uint32_t len = static_cast<uint32_t>(value.size());
    serialize(out, len);
    out.write(value.data(), len);
}

// Overload for bool — avoid the 1-byte bitfield trap
void serialize(std::ostream& out, bool value) {
    uint8_t b = value ? 1 : 0;
    serialize(out, b);
}
// Usage
auto out = std::ofstream("savegame.dat", std::ios::binary);
serialize(out, 42);                     // calls template version
serialize(out, std::string("Hello"));   // calls string overload
serialize(out, true);                   // calls bool overload
```

---

## 1.9 · Explicit Specialization

A full explicit specialization completely replaces the template for one specific type. 
This is useful when the generic implementation doesn't work for a particular type or when you want to optimize for a specific case.
**Disadvantage**: the specialized version doesn't benefit from changes to the primary template and can lead to code duplication if not used carefully.

> **Specialization** differs from **instantiation**: specialization is a separate implementation for a specific type, while instantiation is the process of generating code from the primary template for a given set of template arguments.

```cpp
// Primary: generic hash for trivially copyable types
template <typename T>
size_t asset_hash(const T& v) {
    return std::hash<std::string_view>{}(
        {reinterpret_cast<const char*>(&v), sizeof(T)});
}

// Specialization for string asset paths — delegate to std::hash<string>
template <>
size_t asset_hash<std::string>(const std::string& path) {
    return std::hash<std::string>{}(path);
}

// Specialization for GUID — use only the relevant bytes
template <>
size_t asset_hash<Guid>(const Guid& guid) {
    size_t h;
    memcpy(&h, guid.bytes, sizeof(h));
    return h;
}
```

---

## 1.10 · Overloading Revisited

Function template overloads and specializations interact in a specific order: **non-template functions** win over templates; among templates the most specialized wins; explicit specializations don't participate in overload resolution.

```cpp
// Rule: prefer a non-template if it's an exact match
void draw(Sprite& s);             // non-template — preferred for Sprite
void draw(Text& t);               // non-template — preferred for Text

template <typename T>
void draw(T& renderable) {        // fallback for anything else
    renderable.draw();
}

// When in doubt, explicitly specialize instead of overloading templates
// to avoid subtle ordering surprises.
```

---

## 1.11 · Template Compilation Model

Templates are compiled in two phases:
1. **Phase 1** — the template definition is parsed and checked for syntax, but type-dependent names are not resolved.
2. **Phase 2** — when the template is instantiated, type-dependent names are resolved against the actual type.

> This means template code must be visible at the point of instantiation. The entire implementation usually lives in headers. If you put the template definition in a .cpp, other .cpp files won't see it and instantiation will fail. To work around this, you can use explicit instantiation and `extern` templates to control where code is generated.


```
my_math.hpp       ← template definitions live here (both declaration + body)
main.cpp          ← includes my_math.hpp, triggers instantiation
```

---

## 1.12 · Explicit Instantiation

Explicit instantiation forces the compiler to generate a concrete version in a specific translation unit. This can dramatically speed up builds by avoiding repeated instantiation across dozens of `.cpp` files.

> [!QUESTION] Why do we need explicit instantiation?

> [!IMPORTANT] Because of the two-phase compilation model: the compiler only generates code for a template when it sees an instantiation. If you put the template definition in a header, every .cpp that includes it will generate its own version — leading to long compile times and bloated binaries. By explicitly instantiating in one .cpp and using extern template in others, you can share the same instantiation across the entire program.

```cpp
// math.hpp — declare the template
template <typename T>
T saturate(T v);

// math.cpp — define AND explicitly instantiate
template <typename T>
T saturate(T v) { return clamp(v, T(0), T(1)); }

template float  saturate<float>(float);   // emit the float  version here
template double saturate<double>(double); // emit the double version here
```

```cpp
// In other .cpp files that include math.hpp:
extern template float  saturate<float>(float);   // don't regenerate — use the one from math.cpp
extern template double saturate<double>(double);
```

---

## 1.13 · `extern` Class Templates

`extern template` suppresses instantiation in the current translation unit and tells the linker to expect it elsewhere. Essential for widely-used templates in large codebases.

```cpp
// engine_types.hpp — used in hundreds of .cpp files
extern template class std::vector<RenderCommand>;
extern template class std::vector<Entity>;
extern template class std::unordered_map<AssetId, TextureHandle>;

// engine_types.cpp — instantiate exactly once
template class std::vector<RenderCommand>;
template class std::vector<Entity>;
template class std::unordered_map<AssetId, TextureHandle>;
```

In the sample above you dont have to include `Entity` and `RenderCommand` in the `engine_types.hpp` because you are not using them in that file, you only need to include them in the `engine_types.cpp` where you are instantiating the templates. This way you can reduce compilation dependencies and improve build times.

> So to summarize, when you use extern templates, the extern template is typically declared in a header file so that it is visible when the corresponding header file is included by the clients.

---

## 1.14 · Static Variables in Function Templates

Each instantiation of a function template gets its **own** static variable — one per `T`. This can be useful for per-type singletons or caches, but be careful to avoid confusion with static variables in non-template functions.
```cpp
// Per-type object pool for ECS components
template <typename T>
T* get_component_pool() {
    static T pool[MAX_ENTITIES]; // one pool per component type
    return pool;
}

// PositionComponent pool and VelocityComponent pool are independent
Position* positions = get_component_pool<Position>();
Velocity* velocities = get_component_pool<Velocity>();
```

---

# Section 2 — Class Templates

## 2.1 · Introduction to Class Templates

A class template defines a blueprint for a whole class. Every member function, static variable, and nested type can reference the template parameters. 

> [!IMPORTANT] Compilers act different with class templates than function templates, they don't generate code until you actually use the class (e.g., create an object or call a static member). This allows for more complex and flexible designs, such as type-safe handles, event buses, and compile-time data structures.

```cpp
// A type-safe handle for any engine resource
template <typename ResourceTag>
class Handle {
    uint32_t index_    : 20;
    uint32_t version_  : 12;
public:
    explicit Handle(uint32_t idx, uint32_t ver) : index_(idx), version_(ver) {}
    bool     valid()   const { return index_ != 0; }
    uint32_t index()   const { return index_; }
    uint32_t version() const { return version_; }

    bool operator==(const Handle& o) const {
        return index_ == o.index_ && version_ == o.version_;
    }
};

// Each tag creates a completely distinct type — no accidental mixing
struct TextureTag {};
struct MeshTag    {};
struct ShaderTag  {};

using TextureHandle = Handle<TextureTag>;
using MeshHandle    = Handle<MeshTag>;
using ShaderHandle  = Handle<ShaderTag>;
```

---

## 2.2 · Class Template Examples — A Typed Event Bus

```cpp
// Strongly typed event dispatcher
template <typename EventType>
class EventBus {
    using Listener = std::function<void(const EventType&)>;
    std::vector<Listener> listeners_;

public:
    void subscribe(Listener fn) {
        listeners_.push_back(std::move(fn));
    }

    void publish(const EventType& event) const {
        for (const auto& fn : listeners_)
            fn(event);
    }
};

// Independent buses per event type — zero-overhead per unrelated event
EventBus<PlayerDied>      playerDeathBus;
EventBus<LevelLoaded>     levelBus;
EventBus<ControllerInput> inputBus;

playerDeathBus.subscribe([](const PlayerDied& e) {
    spawnRespawnTimer(e.playerId);
});
playerDeathBus.publish(PlayerDied{ .playerId = 42 });
```

---

## 2.3 · Class Template Argument Deduction (CTAD) — C++17

Since C++17, the compiler can deduce class template arguments from constructor arguments — no more explicit angle brackets in simple cases.

```cpp
template <typename T, size_t N>
struct StaticArray {
    T data[N];
    StaticArray(std::initializer_list<T> init) { /* ... */ }
};

// Before CTAD (C++14 and earlier):
StaticArray<float, 4> clearColor = {0.1f, 0.1f, 0.1f, 1.0f};

// With CTAD deduction guides (C++17):
template <typename T, typename... Rest>
StaticArray(T, Rest...) -> StaticArray<T, 1 + sizeof...(Rest)>;

StaticArray clearColor = {0.1f, 0.1f, 0.1f, 1.0f}; // deduced: StaticArray<float, 4>
```

---

## 2.4 · `extern` Class Templates

Same principle as `extern` function templates — suppress per-TU instantiation.

```cpp
// Widely used pool type — instantiate once
extern template class ObjectPool<Particle>;
extern template class ObjectPool<RigidBody>;
```

---

## 2.5 · Static Members in Class Templates

Each instantiation gets its **own** static data members.

```cpp
template <typename T>
class ComponentRegistry {
    static inline std::vector<T*> instances_;
    static inline uint32_t        typeId_ = nextTypeId();

public:
    static uint32_t typeId()                     { return typeId_; }
    static void     registerInstance(T* ptr)     { instances_.push_back(ptr); }
    static const std::vector<T*>& all()          { return instances_; }
};

// Each component type gets its own registry, its own typeId
// ComponentRegistry<Transform>::typeId() != ComponentRegistry<Rigidbody>::typeId()
```

---

## 2.6 · Explicit Specialization of Member Functions

You can specialize just one member function without specializing the entire class.

```cpp
template <typename T>
class Serializer {
public:
    void write(const T& v, BinaryStream& s) {
        s.write(&v, sizeof(T));
    }
};

// Specialization only for string — rest of the class is unchanged
template <>
void Serializer<std::string>::write(const std::string& v, BinaryStream& s) {
    uint32_t len = static_cast<uint32_t>(v.size());
    s.write(&len, sizeof(len));
    s.write(v.data(), len);
}
```

---

## 2.7 · Explicit Specialization of Class Templates

Full class specialization replaces everything for a specific type.

```cpp
// Primary: generic component storage (SoA-style)
template <typename T>
class ComponentStore {
    std::vector<T> data_;
public:
    void add(const T& c)  { data_.push_back(c); }
    T&   get(size_t idx)  { return data_[idx]; }
    size_t size() const   { return data_.size(); }
};

// Full specialization for Transform — use a custom aligned allocator
template <>
class ComponentStore<Transform> {
    AlignedBuffer<Transform, 16> data_; // SIMD-aligned
public:
    void add(const Transform& t) { data_.append(t); }
    Transform& get(size_t idx)   { return data_[idx]; }
    size_t size() const          { return data_.count(); }
};
```

---

## 2.8 · Non-Type Class Template Arguments

Embed constants directly in the type — no runtime storage, no virtual dispatch.

```cpp
// Compile-time sized matrix — used in shader constant buffers
template <typename T, int Rows, int Cols>
struct Matrix {
    T m[Rows][Cols]{};

    constexpr int rows() const { return Rows; }
    constexpr int cols() const { return Cols; }

    Matrix operator*(const Matrix<T, Cols, Rows>& rhs) const { /* ... */ }
};

using Mat4  = Matrix<float, 4, 4>;
using Mat3  = Matrix<float, 3, 3>;
using Mat34 = Matrix<float, 3, 4>; // common in skinning: 3×4 bone transform

Mat4 view, proj;
Mat4 viewProj = view * proj; // dimensions checked at compile time
```

---

## 2.9 · Partial Specialization

Specialize a template for a *family* of types rather than just one.

```cpp
// Primary: generic serialization
template <typename T>
struct BinaryIO {
    static void write(std::ostream& out, const T& v) {
        out.write(reinterpret_cast<const char*>(&v), sizeof(T));
    }
};

// Partial specialization for any std::vector<T>
template <typename T>
struct BinaryIO<std::vector<T>> {
    static void write(std::ostream& out, const std::vector<T>& v) {
        uint32_t sz = static_cast<uint32_t>(v.size());
        BinaryIO<uint32_t>::write(out, sz);
        for (const T& elem : v)
            BinaryIO<T>::write(out, elem);
    }
};

// Partial specialization for raw arrays T[N]
template <typename T, size_t N>
struct BinaryIO<T[N]> {
    static void write(std::ostream& out, const T (&arr)[N]) {
        for (const T& elem : arr)
            BinaryIO<T>::write(out, elem);
    }
};
// Usage
auto out = std::ofstream("savegame.dat", std::ios::binary);
BinaryIO<int>::write(out, 42);                 // calls primary template
BinaryIO<std::vector<float>>::write(out, vec); // calls vector specialization
BinaryIO<char[256]>::write(out, name);         // calls array specialization
```

> [!QUESTION] How you can separate the declaration and definition of a class template with partial specialization among different tranlation units?

> [!IMPORTANT] You can declare the primary template and the partial specializations in a header file, and then define the member functions in a corresponding .cpp file. However, since templates are typically defined in headers to allow for instantiation across multiple translation units, you would need to ensure that the definitions of the member functions for the primary template and the partial specializations are included in the header as well. Alternatively, you can use explicit instantiation to control where the code is generated, but this is less common for class templates with partial specialization.

Example
**`binary_io.hpp`**
```cpp
// binary_io.hpp — declare primary template and partial specializations
template <typename T>
struct BinaryIO {
    static void write(std::ostream& out, const T& v);
};
template <typename T>
struct BinaryIO<std::vector<T>> {
    static void write(std::ostream& out, const std::vector<T>& v);
};
template <typename T, size_t N>
struct BinaryIO<T[N]> {
    static void write(std::ostream& out, const T (&arr)[N]);
};
```
**`binary_io.cpp`**
```cpp
// binary_io.cpp — define member functions
#include "binary_io.hpp"

template <typename T>
void BinaryIO<T>::write(std::ostream& out, const T& v) {
    out.write(reinterpret_cast<const char*>(&v), sizeof(T));
}
template <typename T>
void BinaryIO<std::vector<T>>::write(std::ostream& out, const std::vector<T>& v) {
    // Same implementation as before
}
template <typename T, size_t N>
void BinaryIO<T[N]>::write(std::ostream& out, const T (&arr)[N]) {
    for (const T& elem : arr)
        BinaryIO<T>::write(out, elem);
}
```

---

# Section 3 — Passing Arguments

## 3.1 · Parameter Types

How you pass template arguments matters — it affects copies, references, and lifetime.

| Declaration | What Happens |
|-------------|--------------|
| `T v` | always copies; `T` deduced without references/const |
| `T& v` | lvalue reference; `T` deduced without reference |
| `const T& v` | safe read-only; avoids copies for large types |
| `T* v` | pointer; deduction strips const from pointee |
| `T&& v` | forwarding reference (see below) |

```cpp
// BAD — copies every asset path string on every lookup
template <typename T> T* findAsset(std::string path);

// GOOD — const ref avoids copies, works with string_view too
template <typename T> T* findAsset(std::string_view path);

// BAD — pointer semantics are awkward for value types
template <typename T> void process(T* value);

// GOOD — pass by value or const ref depending on size
template <typename T> void process(T value);        // small types (int, float)
template <typename T> void process(const T& value); // large types (std::string, Mesh)

// BAD — forces rvalue semantics, can't bind to lvalues
template <typename T> void emplace(T&& component);

emplace(Transform{position, rotation, scale}); // OK — rvalue

Transform t{position, rotation, scale};
emplace(t); // ERROR — can't bind lvalue to T&&

// GOOD — perfect forwarding preserves value category
template <typename T, typename... Args>
void emplace(Args&&... args) {
    // Forward to the actual storage mechanism, perfectly preserving lvalue/rvalue
    store.emplace_back(std::forward<Args>(args)...);
}
emplace(Transform{position, rotation, scale}); // OK — rvalue
emplace(t); // OK — lvalue forwarded as lvalue

```

> **Summary:** Use const T& for read-only parameters, T for small value types, and T&& + std::forward for factory/emplace functions.
---

## 3.2 · Forwarding References

A forwarding reference (`T&&` in a deduced context) binds to both lvalues and rvalues, then `std::forward` preserves the value category.

```cpp
// Generic emplace into an entity component store
template <typename Component, typename... Args>
Component& addComponent(EntityId entity, Args&&... args) {
    auto& store = getStore<Component>();
    return store.emplace(entity, std::forward<Args>(args)...);
}

// Works for both lvalues and rvalues — no unnecessary copies
Transform t{position, rotation, scale};
addComponent<Transform>(entity, t);                // lvalue — copied
addComponent<Velocity>(entity, Vec3{0, 0, 0});     // rvalue — moved
addComponent<Mesh>(entity, std::move(heavyMesh));  // explicit move
```

The rule: use `T&&` + `std::forward<T>` in a deduced context when building *factory* or *emplace* style functions.

Perfect forwarding preserves value category. If you pass an lvalue, it stays an lvalue; if you pass an rvalue, it stays an rvalue. This allows the called function to optimize for move semantics when possible, while still accepting lvalues without forcing unnecessary moves or copies.
Use cases; wrappers for profiling, tracing, jobs, and command dispatch.

You use `decltype(auto)` as the return type to perfectly forward the return value of the callable, preserving its value category (lvalue or rvalue) and const-qualification. This allows the wrapper function to be as transparent as possible, not interfering with move semantics or const-correctness of the original callable.

```cpp
template<typename Fn, typename... Args>
decltype(auto) ProfileCall(Fn&& fn, Args&&... args) {
    return std::forward<Fn>(fn)(std::forward<Args>(args)...);
}
// Usage
ProfileCall([](int x) { return x * 2; }, 21); // returns 42, perfectly forwarding the lambda and its argument

// In a more complex example, you might have a function that takes a callable and its arguments, and you want to forward them to another function that does the actual work, while also adding some profiling logic around it.
template<typename Fn, typename... Args>
decltype(auto) ProfileCall(Fn&& fn, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    decltype(auto) result = std::forward<Fn>(fn)(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Call took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    return result;
}
// Usage
// returns 42 and prints the time taken to execute the lambda
ProfileCall([](int x) { return x * 2; }, 21);

// use regular functions as well
int add(int a, int b) { return a + b; }
ProfileCall(add, 10, 32); // returns 42 and prints the time taken to execute add(10, 32)

// This pattern allows you to write generic profiling code
// that can wrap any callable with any arguments, without 
// losing the benefits of perfect forwarding.

```

---

# Section 4 — Template Changes in C++11

## 4.1 · Right Angle Bracket

Pre-C++11: `vector<vector<int>>` was a parse error (read as `>>`). The compiler now handles this correctly.

```cpp
// C++03 — required a space
std::map<std::string, std::vector<int> > old;

// C++11 and later — natural syntax
std::map<std::string, std::vector<int>> components;
std::unordered_map<EntityId, std::vector<ComponentId>> entityComponentMap;
```
> [!WARNING] Note that MSVC compiler has issues with parsing >> in templates, before C++20 versions, so you may still need to add a space in some cases or switch to at least C++20 version when using MSVC.

---

## 4.2 · Type Aliases & Alias Templates

`using` replaces `typedef` and extends it to support templates.

```cpp
// Old style — typedef can't be templated
typedef std::unordered_map<std::string, int> StringIntMap;

// New style — can be templated
template <typename Value>
using AssetMap = std::unordered_map<AssetId, Value>;

AssetMap<Texture> textures;
AssetMap<Mesh>    meshes;
AssetMap<Shader>  shaders;

// Alias for a component pool type
template <typename T>
using Pool = std::vector<T, AlignedAllocator<T, 64>>; // cache-line aligned

Pool<Transform>  transforms;
Pool<RigidBody>  bodies;
```

---

## 4.3 · Variadic Templates

Accept an arbitrary number of template parameters with `typename...` and `Args...`.

```cpp
// A compile-time type list — the backbone of ECS archetype systems
template <typename... Components>
struct Archetype {
    static constexpr size_t count = sizeof...(Components);

    // Check if a type is in this archetype
    template <typename T>
    static constexpr bool has() {
        return (std::is_same_v<T, Components> || ...); // fold (C++17)
    }
};

using MovableArchetype = Archetype<Transform, Velocity, RigidBody>;
using RenderArchetype  = Archetype<Transform, Mesh, Material>;

static_assert(MovableArchetype::has<Velocity>());
static_assert(!MovableArchetype::has<Mesh>());
```

---

## 4.4 · Variadic Template Example — Building a Tuple-Like Container

```cpp
// Heterogeneous component bundle — stores one of each given component type
template <typename... Ts>
class ComponentBundle {
    std::tuple<Ts...> components_;

public:
    template <typename T>
    T& get() { return std::get<T>(components_); }

    template <typename T>
    const T& get() const { return std::get<T>(components_); }

    template <typename Fn>
    void forEachComponent(Fn&& fn) {
        std::apply([&](auto&... comp) { (fn(comp), ...); }, components_);
    }
};

ComponentBundle<Transform, Velocity, Health> player;
player.get<Health>().hp = 100;
player.forEachComponent([](auto& c) { c.reset(); });
```

---

## 4.5 · `sizeof...` Operator

Returns the number of parameters in a parameter pack — evaluated at compile time.

```cpp
template <typename... Systems>
class SystemScheduler {
    static_assert(sizeof...(Systems) > 0, "At least one system is required");
    static constexpr size_t systemCount = sizeof...(Systems);

public:
    void tickAll(float dt) {
        // C++17 fold — runs each system's tick
        (Systems::tick(dt), ...);
    }
};

SystemScheduler<PhysicsSystem, AnimationSystem, AISystem, AudioSystem> scheduler;
// systemCount == 4 — known at compile time
```

---

## 4.6 · Variadic Classes — Building an Observer

```cpp
// Type-safe multi-event listener
template <typename... Events>
class MultiListener : public Listener<Events>... {
public:
    using Listener<Events>::onEvent...;
};

class GameHUD : public MultiListener<PlayerDied, ScoreChanged, LevelComplete> {
    void onEvent(const PlayerDied& e)    override { showDeathScreen(e.playerId); }
    void onEvent(const ScoreChanged& e)  override { updateScoreDisplay(e.score); }
    void onEvent(const LevelComplete& e) override { showVictoryScreen(); }
};
```

---

## 4.7 · Variadic Template Applications — `make_component`

```cpp
// Perfect-forward any number of args to construct a component in-place
template <typename T, typename... Args>
T& World::emplace(EntityId entity, Args&&... args) {
    auto& pool = getPool<T>();
    pool[entity.index()] = T(std::forward<Args>(args)...);
    entityMask_[entity.index()].set(ComponentId<T>::value);
    return pool[entity.index()];
}

// Construct a RigidBody directly in the pool — no copies, no temporaries
world.emplace<RigidBody>(entity,
    /*mass=*/  10.0f,
    /*shape=*/ CollisionShape::Box,
    /*flags=*/ RigidBodyFlags::Dynamic | RigidBodyFlags::CastsShadow);
```

---

# Section 5 — Template Changes in C++14 & C++17

## 5.1 · Variable Templates (C++14)

Template a *variable* — not just a function or class.

```cpp
// Type-safe Pi for any precision
template <typename T>
inline constexpr T Pi = T(3.14159265358979323846264338327950288L);

float  angle = Pi<float>  * 2.0f;   // single-precision circle
double angle = Pi<double> * 2.0;    // double-precision for physics

// Component type ID — one unique int per type, evaluated once
template <typename T>
inline uint32_t ComponentTypeId = ComponentRegistry::nextId();
```

---

## 5.2 · Generic Lambdas (C++14)

`auto` parameters in lambdas create anonymous function templates.

```cpp
// Generic sorter — works for any container of anything with a .priority field
auto byPriority = [](const auto& a, const auto& b) {
    return a.priority > b.priority;
};

std::sort(renderQueue.begin(),   renderQueue.end(),   byPriority);
std::sort(particleEffects.begin(), particleEffects.end(), byPriority);
std::sort(audioEvents.begin(),   audioEvents.end(),   byPriority);
```

---

## 5.3 · `if constexpr` (C++17)

A compile-time branch that *discards* the unchosen branch entirely — the discarded branch is never compiled.

```cpp
// Serialize a component differently based on its type traits
template <typename T>
void writeComponent(BinaryWriter& w, const T& component) {
    if constexpr (std::is_trivially_copyable_v<T>) {
        // Fast path: memcpy the whole struct
        w.writeRaw(&component, sizeof(T));
    } else if constexpr (requires { component.serialize(w); }) {
        // The component knows how to serialize itself
        component.serialize(w);
    } else {
        static_assert(sizeof(T) == 0, "Component must be trivially copyable or have serialize()");
    }
}
```

Without `if constexpr`, you'd need separate overloads or tag dispatch for every case.

---

## 5.4 · Fold Expressions (C++17)

Apply a binary operator across all elements of a parameter pack.
Used in debug consoles, telemetry, and diagnostics.
| Syntax | Meaning |
|--------|---------|
| `(... op pack)` | Left fold: `((a op b) op c)` |
| `(pack op ...)` | Right fold: `(a op (b op c))` |
| `(init op ... op pack)` | Left fold with initial value |
| `(pack op ... op init)` | Right fold with initial value |

```cpp
// Does an entity have ALL of the given component types?
template <typename... Required>
bool World::hasAll(EntityId e) const {
    return (entityMask_[e.index()].test(ComponentId<Required>::value) && ...);
}
// Example usage:
if (world.hasAll<Transform, Velocity>(entity)) {
    // This entity has both Transform and Velocity components
}

// Register a system that requires a specific set of components
template <typename... Components>
void World::registerSystem(auto&& system) {
    auto view = query<Components...>(); // generates a typed view at compile time
    system.run(view);
}
// Example usage:
world.registerSystem<Transform, Mesh>(renderSystem);

// Sum all component weights — e.g. inventory mass calculation
template <typename... Items>
float totalMass(const Items&... items) {
    return (items.mass + ... + 0.0f);
}
// Example usage:
float mass = totalMass(sword, shield, potion); // sums the mass of all items

// Debug print all component types of an entity
template <typename... Components>
void debugPrint(EntityId e) {
    std::cout << "Entity " << e.id() << " has components: ";
    ((std::cout << typeid(Components).name() << " "), ...);
    std::cout << "\n";
}
// Example usage:
debugPrint<Transform, Velocity, Health>(entity);

// Telemetry: log multiple values with a single call
template <typename... Values>
void logTelemetry(const Values&... vals) {
    (log(vals), ...); // calls log() for each value
}
// Example usage:
logTelemetry(player.position, player.health, player.score);

// Diagnostics: check multiple conditions at once
template <typename... Checks>
bool allChecksPass(const Checks&... checks) {
    return (checks() && ...); // calls each check and ANDs the results
}
// You can even limit Checks to be lambdas/functions that return bool, 
// for better readability using concepts like this:
concept BoolCheck = std::invocable<Checks> && // checks must be callable with no arguments
                    std::same_as<std::invoke_result_t<Checks>, bool>; // checks must return bool
template <BoolCheck... Checks>
bool allChecksPass(const Checks&... checks) {
    return (checks() && ...); // calls each check and ANDs the results
}

// Example usage:
// Note that you can use regular if checks instead of lambdas if you prefer, 
// but lambdas allow for more complex logic and better readability in this context.
if (allChecksPass(
    [] { return player.health > 0; },
    [] { return !player.isInvulnerable(); },
    [] { return enemyCount < 5; }
)) {
    // All conditions are met — safe to spawn more enemies
}
```

---

## 5.5 · `auto` Type Placeholder in Template Context (C++17)

`auto` can appear as a non-type template argument placeholder.

```cpp
// Before C++17 — explicit type needed
template <typename T, T Value>
struct CompileTimeConstant {};

// C++17 — let the compiler deduce it
template <auto Value>
struct CompileTimeConstant {
    using type = decltype(Value);
    static constexpr type value = Value;
};

CompileTimeConstant<42>        intConst;    // type = int
CompileTimeConstant<3.14f>     floatConst;  // type = float
CompileTimeConstant<'X'>       charConst;   // type = char

// Practical: enum values as template parameters
enum class ShaderStage { Vertex, Fragment, Compute };
template <auto Stage>
class ShaderModule { /* ... */ };

ShaderModule<ShaderStage::Vertex>   vsModule;
ShaderModule<ShaderStage::Compute>  csModule;
```

---

## 5.6 · Compile-time `if` with `constexpr` Functions (C++14+)

Extend the compile-time evaluation chain deeper:

```cpp
// Compute aligned buffer size at compile time
template <typename T, size_t MinAlign = 16>
constexpr size_t alignedSizeOf() {
    constexpr size_t base = sizeof(T);
    if constexpr (alignof(T) >= MinAlign) {
        return base;
    } else {
        return (base + MinAlign - 1) & ~(MinAlign - 1);
    }
}

static_assert(alignedSizeOf<uint8_t, 16>() == 16);
static_assert(alignedSizeOf<float[4], 16>() == 16);
```

---

# Section 6 — Template Changes in C++20

## 6.1 · Non-Type Template Parameters — Expanded (C++20)

C++20 allows floating-point types, class types with `constexpr` constructors, and structural types as non-type template parameters.

```cpp
// Compile-time color — no more magic numbers in pipeline descriptions
struct Color {
    float r, g, b, a;
    constexpr bool operator==(const Color&) const = default; // structural type requirement
};

template <Color ClearColor>
class RenderPass {
public:
    void begin(CommandBuffer& cmd) {
        cmd.clearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);
    }
};

constexpr Color SkyBlue  = {0.53f, 0.81f, 0.92f, 1.0f};
constexpr Color Midnight = {0.10f, 0.10f, 0.20f, 1.0f};

RenderPass<SkyBlue>  dayPass;
RenderPass<Midnight> nightPass;
```

C++20 broadens what can be used as non-type template parameters.
This can be used to create unique types based on values, which can be useful for things like type-safe identifiers or compile-time configuration like compile-time tags for messages/events/resources.

```cpp  
template<auto Id>
struct Message {
    static constexpr auto id = Id;
    // message data and methods...
};
Message<42> msgA; // Message with ID 42
Message<100> msgB; // Message with ID 100
```

---

## 6.2 · Template Parameters in Lambda Expressions (C++20)

Give lambda template parameters their own explicit names — you're no longer limited to `auto`.

```cpp
// Before C++20 — auto params, but no way to name the type
auto oldMultiply = [](auto a, auto b) { return a * b; };

// C++20 — explicit template syntax in lambdas
auto multiply = []<typename T>(T a, T b) -> T { return a * b; };

// Enforce both parameters are the same type
auto strictAdd = []<typename T>(T a, T b) { return a + b; };
// strictAdd(1, 2.0); // won't compile — different types

// Access the type for further operations
auto componentCast = []<typename To, typename From>(From* ptr) -> To* {
    static_assert(std::is_base_of_v<From, To>, "Invalid component cast");
    return static_cast<To*>(ptr);
};
```

---

## 6.3 · Abbreviated `auto` Function Templates (C++20)

`auto` parameters in regular functions create implicit function templates — no `template<>` needed.

```cpp
// These are equivalent:
template <typename T>
void applyForce(T& body, Vec3 force) { body.addForce(force); }

// C++20 abbreviated form — reads like a normal function
void applyForce(auto& body, Vec3 force) { body.addForce(force); }

// Mixed — some types known, one deduced
void setUniform(ShaderProgram& prog, std::string_view name, const auto& value) {
    prog.setUniform(name, value); // works for float, Vec3, Mat4, int, ...
}

// In a render loop:
setUniform(shader, "uViewProj", viewProjMatrix);  // Mat4
setUniform(shader, "uTime",     elapsedTime);     // float
setUniform(shader, "uLightPos", lightPosition);   // Vec3
```

---

## 6.4 · Concepts — Constrained Templates (C++20)

Concepts let you express *requirements* on template parameters with readable syntax and clear error messages.

```cpp
// Define reusable constraints
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T>
concept Drawable = requires(T obj, CommandBuffer& cmd) {
    { obj.draw(cmd) } -> std::same_as<void>;
    { obj.boundingBox() } -> std::convertible_to<AABB>;
};

template <typename T>
concept ECSComponent = std::is_trivially_destructible_v<T>
                    && std::is_move_constructible_v<T>;

// Use concepts to constrain templates
template <Numeric T>
T lerp(T a, T b, float t) { return a + (b - a) * t; }

// Requires clause in template head, this tells the compiler to only instantiate this function if T satisfies Drawable
template <typename T> requires Drawable<T>
void submitToRenderer(RenderQueue& queue, T& obj) {
    if (frustum.contains(obj.boundingBox()))
        queue.push(&obj);
}

// Here we tell the compiler that T must satisfy the Component concept, 
// which means it must have a nested type called ComponentTag.
// This allows us to ensure that only valid component types can be registered in our ECS system.
template<typename T>
concept Component = requires {
    typename T::ComponentTag;
};
template<Component... Cs>
class Query; // Query can only be instantiated with types that satisfy the Component concept

// Concepts combined
template <ECSComponent T>
void World::registerComponent() {
    static_assert(!std::is_polymorphic_v<T>,
        "ECS components must not be polymorphic — use composition");
    componentPools_.emplace_back(std::make_unique<ComponentPool<T>>());
}
```

Before Concepts you'd get walls of cryptic template errors. Now, violating a constraint gives you a human-readable diagnostic.

---

## 6.5 · `requires` Expressions

A `requires` expression lets you test arbitrary well-formedness conditions inline.

```cpp
// Is T serializable to a BinaryWriter?
template <typename T>
concept Serializable = requires(const T& v, BinaryWriter& w) {
    { v.serialize(w) } -> std::same_as<void>;
};

// Does T support SIMD-friendly iteration?
template <typename T>
concept SIMDFriendly = requires {
    requires std::is_trivially_copyable_v<T>;
    requires sizeof(T) % 16 == 0;
    requires alignof(T) >= 16;
};

// Conditional implementation via constrained overloads
template <SIMDFriendly T>
void transformBatch(T* data, size_t count, const Mat4& m) {
    // SIMD path
}

template <typename T>  // unconstrained fallback
void transformBatch(T* data, size_t count, const Mat4& m) {
    // scalar path
}
```

---

## Quick Reference — Template Feature Timeline

| Feature | Standard | Game Engine Use Case |
|---------|----------|----------------------|
| Function & class templates | C++98 | Math library, containers |
| Explicit specialization | C++98 | Custom serialization per type |
| Partial specialization | C++98 | BinaryIO for vectors, arrays |
| `extern template` | C++11 | Suppress repeated STL instantiation |
| Variadic templates | C++11 | ECS archetypes, event buses |
| `sizeof...` | C++11 | Compile-time component counts |
| Alias templates (`using`) | C++11 | Typed pools, typed handles |
| Variable templates | C++14 | `Pi<T>`, component type IDs |
| Generic lambdas | C++14 | Reusable comparators, callbacks |
| `if constexpr` | C++17 | Branch on type traits without overloads |
| Fold expressions | C++17 | `hasAll<T...>`, mass sums |
| CTAD | C++17 | Shorter construction syntax |
| `auto` non-type params | C++17 | Enum values as template params |
| Expanded non-type params | C++20 | Color, Vec3 as template params |
| Lambda template params | C++20 | Named types inside lambdas |
| Abbreviated `auto` | C++20 | Concise generic functions |
| Concepts & `requires` | C++20 | Readable constraints, better errors |

---

*All samples in this guide are illustrative; adapt type names and APIs to match your engine's architecture.*
