#include "CastSample.hpp"
#include <iostream>
#include <bit>
#include <array>
#include <memory>
#include <vector>

// Helper classes for demonstration
class CastSample::Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() const { std::cout << "Some animal sound\n"; }
};

class CastSample::Dog : public Animal {
public:
    void speak() const override { std::cout << "Woof!\n"; }
    void fetch() const { std::cout << "Dog fetches ball\n"; }
};

class CastSample::Cat : public Animal {
public:
    void speak() const override { std::cout << "Meow!\n"; }
    void scratch() const { std::cout << "Cat scratches furniture\n"; }
};

void CastSample::demonstrate_static_cast() {
    std::cout << "\n=== static_cast - Safe Compile-Time Cast ===\n";

    // Numeric conversions
    double pi = 3.14159;
    int rounded = static_cast<int>(pi);
    std::cout << "double " << pi << " -> int " << rounded << " (truncated)\n";

    // Upcasting (derived -> base) - always safe
    Dog* dog = new Dog();
    Animal* animal = static_cast<Animal*>(dog);
    std::cout << "Dog* -> Animal*: ";
    animal->speak();

    // Downcasting (base -> derived) - unsafe if wrong type
    Animal* another_animal = new Dog();
    Dog* another_dog = static_cast<Dog*>(another_animal);
    std::cout << "Animal* -> Dog*: ";
    another_dog->fetch();

    // Converting void* back to original type
    void* data = dog;
    Dog* restored_dog = static_cast<Dog*>(data);
    std::cout << "void* -> Dog*: ";
    restored_dog->speak();

    delete dog;
    delete another_animal;
}

void CastSample::demonstrate_dynamic_cast() {
    std::cout << "\n=== dynamic_cast - Runtime-Checked Cast ===\n";

    std::vector<Animal*> animals;
    animals.push_back(new Dog());
    animals.push_back(new Cat());
    animals.push_back(new Dog());

    for (Animal* animal : animals) {
        std::cout << "Animal says: ";
        animal->speak();

        // Try to cast to Dog
        if (Dog* dog = dynamic_cast<Dog*>(animal)) {
            std::cout << "It's a Dog! ";
            dog->fetch();
        } else if (Cat* cat = dynamic_cast<Cat*>(animal)) {
            std::cout << "It's a Cat! ";
            cat->scratch();
        }
    }

    // With references - throws std::bad_cast on failure
    try {
        Animal& animal_ref = *animals[0];
        Dog& dog_ref = dynamic_cast<Dog&>(animal_ref);
        std::cout << "Reference cast successful: ";
        dog_ref.fetch();
    } catch (const std::bad_cast&) {
        std::cout << "Reference cast failed - not a Dog\n";
    }

    for (auto* animal : animals) delete animal;
}

void CastSample::demonstrate_const_cast() {
    std::cout << "\n=== const_cast - Adding/Removing const ===\n";

    const std::string const_str = "Hello, World!";
    std::cout << "Original const string: " << const_str << "\n";

    // Remove const (dangerous - only if you know it's safe!)
    std::string* modifiable = const_cast<std::string*>(&const_str);
    // DON'T MODIFY: *modifiable = "Goodbye!"; // Undefined behavior!

    std::cout << "const_cast successful - but don't modify const data!\n";

    // Add const
    std::string mutable_str = "Mutable";
    const std::string* const_ptr = const_cast<const std::string*>(&mutable_str);
    std::cout << "Added const: " << *const_ptr << "\n";
}

void CastSample::demonstrate_reinterpret_cast() {
    std::cout << "\n=== reinterpret_cast - Bit Reinterpretation ===\n";

    // Type punning for bit manipulation
    float f = 3.14f;
    uint32_t* bits = reinterpret_cast<uint32_t*>(&f);
    std::cout << "float " << f << " as uint32_t: " << *bits << "\n";

    // Converting between pointer types
    uintptr_t address = 0xDEADBEEF;
    int* ptr = reinterpret_cast<int*>(address);
    std::cout << "uintptr_t " << std::hex << address << " -> int*: " << ptr << "\n";

    std::cout << "WARNING: reinterpret_cast is very dangerous!\n";
}

void CastSample::demonstrate_bit_cast() {
    std::cout << "\n=== std::bit_cast (C++20) - Safe Bit Reinterpretation ===\n";

    float f = 1.0f;
    uint32_t bits = std::bit_cast<uint32_t>(f);
    std::cout << "float " << f << " -> uint32_t: " << bits << "\n";

    // Reverse
    float restored = std::bit_cast<float>(bits);
    std::cout << "uint32_t " << bits << " -> float: " << restored << "\n";

    // With arrays
    std::array<std::byte, 4> bytes = {std::byte{0x00}, std::byte{0x00}, std::byte{0x80}, std::byte{0x3F}};
    float value = std::bit_cast<float>(bytes);
    std::cout << "byte array -> float: " << value << "\n";
}

void CastSample::demonstrate_casting_best_practices() {
    std::cout << "\n=== Casting Best Practices ===\n";

    std::cout << "1. Prefer static_cast for most conversions\n";
    std::cout << "2. Use dynamic_cast only with polymorphic types\n";
    std::cout << "3. Avoid const_cast when possible\n";
    std::cout << "4. Use reinterpret_cast only for low-level code\n";
    std::cout << "5. Prefer std::bit_cast over reinterpret_cast for bit manipulation\n";
    std::cout << "6. Avoid C-style casts: (int)x - use static_cast<int>(x)\n";
    std::cout << "7. Document why you're casting and why it's safe\n";
}

void CastSample::demonstrate_best_practices() {
    std::cout << "\n=== Modern C++ Casting Guidelines ===\n";

    // Example: Safe upcasting
    std::unique_ptr<Dog> dog = std::make_unique<Dog>();
    std::unique_ptr<Animal> animal = std::move(dog); // No cast needed!

    std::cout << "Smart pointers handle inheritance automatically: ";
    animal->speak();

    // Example: When you need downcasting
    Animal* base_ptr = new Dog();
    if (Dog* derived_ptr = dynamic_cast<Dog*>(base_ptr)) {
        std::cout << "Safe downcast successful: ";
        derived_ptr->fetch();
    }

    delete base_ptr;
}

void CastSample::run() {
    std::cout << "Running C++ Cast Types Sample...\n";

    demonstrate_static_cast();
    demonstrate_dynamic_cast();
    demonstrate_const_cast();
    demonstrate_reinterpret_cast();
    demonstrate_bit_cast();
    demonstrate_casting_best_practices();
    demonstrate_best_practices();

    std::cout << "\n=== Cast Types Summary ===\n";
    std::cout << "static_cast: Safe, compile-time checked conversions\n";
    std::cout << "dynamic_cast: Runtime-checked polymorphic conversions\n";
    std::cout << "const_cast: Add/remove const (dangerous)\n";
    std::cout << "reinterpret_cast: Bit reinterpretation (very dangerous)\n";
    std::cout << "std::bit_cast: Safe bit reinterpretation (C++20)\n";
    std::cout << "\nChoose the right cast for the job - wrong choice = bugs!\n";

    std::cout << "\nC++ Cast Types demonstration completed!\n";
}
