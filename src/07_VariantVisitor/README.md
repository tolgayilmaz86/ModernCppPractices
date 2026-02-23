# Variant and Visitor Pattern

## Overview

The Variant and Visitor pattern combines `std::variant` (C++17) with the visitor pattern to provide type-safe, compile-time polymorphism. This approach offers an alternative to inheritance-based polymorphism when you have a fixed set of types and want to perform different operations on each type.

**The Problem:** Traditional inheritance requires a common base class and virtual functions. What if you want type-safe operations on a discriminated union without the overhead of inheritance?

**The Solution: Variant + Visitor** Use `std::variant` to store different types safely, and the visitor pattern to perform type-specific operations in a type-safe manner.

## Motivation: Value Semantics vs Reference Semantics

When people think about runtime polymorphism in C++, they usually think about `virtual` functions and pointer or reference semantics. But in **modern C++**, we tend to embrace **value semantics** more and more for its efficiency and clarity.

A natural question arises: what do we do if we want runtime flexibility without giving up the power of value semantics?

### The Problem with Templates (Static Polymorphism)

Templates allow us to work with different types that conform to a common interface:

```cpp
struct PngImage {
    void Save(const std::string& file_name) const {
        std::cout << "Saving " << file_name << ".png\n";
    }
};

struct JpegImage {
    void Save(const std::string& file_name) const {
        std::cout << "Saving " << file_name << ".jpg\n";
    }
};

template <typename Image>
void SaveImage(const Image& image, const std::string& file_name) {
    image.Save(file_name);
}
```

But this pattern is not useful if we want to decide which format an image has **at runtime** - all the code with all the concrete types needs to be visible to the compiler at compile time!

### The Problem with Inheritance (Dynamic Polymorphism)

We can use inheritance with virtual functions:

```cpp
struct Saveable {
    virtual void Save(const std::string& file_name) const = 0;
    virtual ~Saveable() = default;
};

struct PngImage : public Saveable {
    void Save(const std::string& file_name) const override {
        std::cout << "Saving " << file_name << ".png\n";
    }
};

std::vector<std::unique_ptr<Saveable>> images;
images.push_back(std::make_unique<PngImage>());
```

But we **did** have to give up certain things:
- Our image classes inherit from a common rigid interface class
- We are forced to embrace reference and pointer semantics
- We cannot easily copy or move the actual objects around
- We have to allocate pointers on the heap

### The Solution: std::variant

This is where `std::variant` comes to the rescue. It allows us to keep using templates just like we originally wanted, but adds a twist. We can store a **variant** of our types in a vector and use `std::visit` to call our `SaveImage` on any type from the ones we allow in the variant:

```cpp
using Image = std::variant<PngImage, JpegImage>;

void SaveImage(const Image& image, const std::string& file_name) {
    std::visit([&](const auto& img) { img.Save(file_name); }, image);
}

int main() {
    const std::vector<Image> images = {PngImage{}, JpegImage{}};
    for (const auto& image : images) SaveImage(image, "output");
}
```

Note how we create the vector in exactly the way that we dreamed about before! We also call the same `SaveImage` function just like we did when using `virtual` functions and pointers!

## How Variant + Visitor Works

1. **Define the Variant:** `std::variant<Type1, Type2, ...>` can hold any of the specified types
2. **Create Visitors:** Functions or functors that handle each type in the variant
3. **Use std::visit:** Applies the visitor to the variant's current value
4. **Type Safety:** All operations are checked at compile time

```cpp
using Shape = std::variant<Circle, Square, Triangle>;

struct DrawVisitor {
    void operator()(const Circle& c) { /* draw circle */ }
    void operator()(const Square& s) { /* draw square */ }
    void operator()(const Triangle& t) { /* draw triangle */ }
};

Shape shape = Circle{5.0};
std::visit(DrawVisitor{}, shape);  // Type-safe dispatch
```
## Key Components

### 1. std::variant

The class `std::variant` is a so-called type-safe `union` type introduced in C++17. A variable of such a variant type holds one value out of a defined set of types.

- **Type-safe union** that can hold one of several specified types
- **Compile-time type checking** prevents invalid operations
- **Fixed memory layout** - size is max of all alternative types
- **Default constructible** with first type, or use `std::monostate` for empty

```cpp
// There can be any number of types in std::variant.
// By default, variant stores a value of the first type.
std::variant<int, std::string, double> value{};
return 0;
```

Intuitively we can imagine `std::variant` as a "box" type in which we can store objects of some selected set of types.

#### Memory Used by std::variant

The values in a variant occupy the same memory, which means that the amount of memory allocated for the whole variant object needs to be enough to store an object of the **biggest type** potentially stored in it (plus some memory to store an index of which value is stored). In our previous example, even when we write an `int` into our variant object, the object still allocates enough memory needed for a `std::string`.

Think of it like a box that needs to be big enough to hold a banana - if we put an apple into it, some space will be left unused.

#### Use std::monostate to Allow for "Empty" Variants

Can the box be empty? In other words, can we have an "empty" `std::variant`?

By default, variants store a value of the first type. So it would seem that creating an empty box would be problematic. But if we think more about it, always creating a variable of the first type to populate a `std::variant` value might not be desirable or, indeed, even possible. Imagine that we store only types that do not have a default constructor in the first place:

```cpp
struct NonDefaultConstructibleType {
    NonDefaultConstructibleType(int) {}
};

// ❌ Can't compile if first type is not default constructible.
// int main() { std::variant<NonDefaultConstructibleType, int, double> v{}; }
```

To mitigate such situations there is a type `std::monostate` in the standard library. This is an empty type that is default-constructible and we can use it as the first type in the list of types that we pass to a `std::variant` should none of our types be default constructible or if we simply want our variant to be in an "empty" state by default:

```cpp
int main() {
    std::variant<std::monostate, NonDefaultConstructibleType, int, double> v{};
    // value holds an instance of std::monostate for now.
}
```

So essentially, in our "fruit" example, an empty box actually holds a `std::monostate` object.

### 2. Visitor Pattern
- **Overloaded call operator** for each type in the variant
- **Type-specific operations** without runtime type checks
- **Can maintain state** across multiple visits
- **Exception safety** through proper error handling

### 3. Storing and Getting Values from std::variant

Once we stored some values in a variant object, we usually want to get them out somehow. The simplest method for getting a value would be to just `get` it by using either the type name or an index of that type:

```cpp
int main() {
    std::variant<int, std::string> value{};
    // By default, variant stores a value of the first type.
    std::cout << std::boolalpha;
    std::cout << "Holds integer: " << std::holds_alternative<int>(value) << '\n';
    std::cout << "Holds string: " << std::holds_alternative<std::string>(value) << '\n';
    std::cout << "Integer: " << std::get<int>(value) << '\n';
    std::cout << "Integer by index: " << std::get<0>(value) << '\n';
    std::cout << "String by index: " << std::get<1>(value) << '\n';
    
    value = 42;  // value holds an int.
    std::cout << "Holds integer: " << std::holds_alternative<int>(value) << '\n';
    std::cout << "Integer: " << std::get<int>(value) << '\n';
    std::cout << "Integer by index: " << std::get<0>(value) << '\n';
    return 0;
}
```

Do note though, that once we put one type into variant, `get`'ing another type will throw a `std::bad_variant_access` exception. If we don't want to use exceptions, we can use `std::holds_alternative` to check which type the variant currently holds before `get`ting its value.

### 4. std::visit
- **Type-safe dispatch** to the appropriate visitor function
- **Compile-time resolution** of which overload to call
- **Works with any callable** (functions, lambdas, functors)
- **Returns the result** of the visitor call

#### How std::visit Selects the Correct Function

The fact that `std::visit` can select the correct function from a variant seems almost magical, but, as always, it is nothing but a clever implementation.

The exact details of how `std::visit` is implemented are probably beyond the scope of today's lecture, but we can quote [cppreference.com](https://en.cppreference.com/w/cpp/utility/variant/visit2.html#Notes) to get the gist of how the appropriate function is selected when `std::visit` is called:

> Implementations usually generate a table equivalent to a possibly multidimensional array of function pointers for every specialization of `std::visit`, which is similar to the implementation of virtual functions.
> On typical implementations, the time complexity of the invocation of the callable can be considered equal to that of access to an element in a possibly multidimensional array or execution of a switch statement.

That is to say: selecting the right function is usually pretty fast but still takes **some** tiny amount of time **at runtime**. And the more different `std::visit` calls there are - the slower every call becomes.

But the main message is that this selection **does** happen at runtime and that means that we **did** implement dynamic polymorphism that still lets us use value semantics (and even built-in types) and typically works pretty fast! 🎉

#### Visitor Must Cover All Variant Types

The whole conversation about `std::visit` would not be complete without talking about one important pitfall of `std::visit` that many beginners struggle with.

You see, we need to ensure that **all** the types in a variant are covered in the function object we provide into the `std::visit`. The code won't compile otherwise.

This might seem confusing at the beginning: why do we have to cover cases that we never aim to use? However, the reason why it was designed this way becomes easier to see if we look at a slightly more complex example.

Imagine for a moment that we have a class `Foo` that holds some `std::variant` member variable and a function `Print`. If we use `std::visit` with a visitor that does not handle all of the types that we can store in our variant, the compiler will reject the code because there would be no way for the `std::visit` compiled into this library's binary file to handle the missing type, should it ever be stored in our variant object.

## Implementation Details

This sample demonstrates:

1. **Basic Variant Usage**: Creating and storing different types
2. **Custom Visitor Classes**: Complex operations with state
3. **Overloaded Lambdas**: Simple visitors using C++17 features
4. **Error Handling**: Type-safe validation and exception handling
5. **Multiple Visitors**: Different operations on the same data
6. **Direct Access Methods**: Using `std::get`, `std::holds_alternative`, and `std::get_if`
7. **Memory Layout**: Understanding variant storage characteristics
8. **Advanced Lambda Visitors**: Using `constexpr` and type traits
9. **Performance Comparison**: Variant vs inheritance approaches

## When to Use Variant + Visitor

✅ **Good for:**
- Fixed set of types known at compile time
- Type-safe discriminated unions
- Avoiding inheritance hierarchies
- Performance-critical code with small type sets
- Adding new operations without modifying existing types
- Strong type safety requirements

❌ **Not good for:**
- Dynamic type sets (types added at runtime)
- Large number of types (complexity grows)
- Simple cases where inheritance works fine
- When you need runtime type identification
- Third-party types you cannot modify

## Performance Characteristics

**Advantages:**
- **No virtual function calls** (compile-time dispatch)
- **Fixed memory layout** (no heap allocation for small types)
- **Better cache locality** than inheritance
- **Type safety** at compile time
- **No inheritance overhead**

**Disadvantages:**
- **Must know all types** at compile time
- **Cannot add types** without recompilation
- **std::visit overhead** (though minimal)
- **Larger binary size** with many types
- **Complex syntax** for simple cases

## Comparison with Other Approaches

| Approach | Inheritance | Variant + Visitor | std::any | Templates |
|----------|-------------|-------------------|----------|-----------|
| Type Safety | Runtime | Compile-time | Runtime | Compile-time |
| Performance | Virtual calls | Direct calls | Type checks | Monomorphized |
| Flexibility | High | Medium | High | Low |
| Complexity | Medium | Medium | Low | High |
| Memory | Vtable ptr | Fixed size | Dynamic | Optimized |

## Advanced Usage Patterns

### 1. Recursive Variants
```cpp
using Expr = std::variant<int, std::unique_ptr<struct BinaryOp>>;
```

### 2. Visitor with Return Types
```cpp
auto result = std::visit([](auto&& arg) -> ReturnType {
    return process(arg);
}, variant);
```

### 3. Generic Visitors
```cpp
template <typename... Visitors>
auto make_visitor(Visitors&&... visitors) {
    return overloaded{std::forward<Visitors>(visitors)...};
}
```

### 4. The Overloaded Helper Pattern

By creating a template `struct` (often called `Overloaded`) that accepts multiple classes, each with an `operator()` (like a function call), we can instantiate an object of this struct by passing in as many function objects (also lambdas) as we need:

```cpp
// Helper type for nicer calling-site syntax
template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

// Explicit deduction guide (not needed as of C++20)
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

int main() {
    std::variant<int, std::string> value{};
    
    const Overloaded visitor{
        [](int arg) { std::cout << "Int: " << arg << '\n'; },
        [](const std::string& arg) { std::cout << "String: " << arg << '\n'; }
    };
    
    std::visit(visitor, value);
    value = "Hello, variant!";
    std::visit(visitor, value);
    value = 42;
    std::visit(visitor, value);
}
```

This lets us easily create a local visitor right where we need it, taking full advantage of the flexibility that lambdas offer while having explicit overloads for types that demand it.

### 5. Using auto in Lambda Visitors

Note how in this example we use `auto` for a type of our `value` in a lambda. This `auto` will become different types depending on which type is actually stored in a variant and the whole example works here because `std::cout` is able to accept any of the types we use here. If we would store a type that `std::cout` does not work with, like `std::monostate`, the situation would look a bit different:

```cpp
int main() {
    std::variant<std::monostate, int, std::string> value{};
    
    const Overloaded visitor{
        [](auto arg) { std::cout << "Unknown type\n"; },  // Catch-all
        [](int arg) { std::cout << "Int: " << arg << '\n'; },
        [](const std::string& arg) { std::cout << "String: " << arg << '\n'; }
    };
    
    std::visit(visitor, value);  // Outputs: Unknown type
    value = "Hello, variant!";
    std::visit(visitor, value);  // Outputs: String: Hello, variant!
    value = 42;
    std::visit(visitor, value);  // Outputs: Int: 42
}
```

Note how here too we can use `auto` to catch any types that we don't want to handle explicitly, but if we **do** decide to handle them explicitly, those implementations are preferred.

## Best Practices

1. **Use std::monostate** for variants that can be empty
2. **Keep variant sizes small** - prefer references for large types
3. **Use overloaded lambdas** for simple visitors
4. **Custom visitor classes** for complex stateful operations
5. **Handle all types** in visitors to avoid compilation errors
6. **Consider inheritance** for very simple cases
7. **Use const references** in visitors when possible

## Modern C++ Integration

The Variant + Visitor pattern integrates well with other Modern C++ features:

- **Smart pointers** for complex types in variants
- **Move semantics** for efficient variant operations
- **constexpr** for compile-time computations
- **Concepts** (C++20) for constraining visitor parameters
- **Coroutines** for async visitor operations

This pattern represents a powerful tool in the Modern C++ toolbox for type-safe, efficient polymorphic operations without inheritance.