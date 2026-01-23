# Extending iostream with Custom Types

## Overview

The C++ iostream library is highly extensible. You can add support for:
1. **Custom types** with `operator<<` and `operator>>`
2. **Custom manipulators** (like `std::hex`, `std::setw`)
3. **Polymorphic serialization** with virtual functions
4. **Factory patterns** for deserializing derived types

This sample uses a **game engine entity system** to demonstrate these concepts.

## 1. Adding `operator<<` (Inserter)

To output a custom type, overload `operator<<`:

```cpp
class GameEntity {
    std::string name_;
    float x_, y_;

    friend std::ostream& operator<<(std::ostream& os, const GameEntity& entity);
    
protected:
    virtual std::ostream& print(std::ostream& os) const {
        os << name_ << " " << x_ << " " << y_;
        return os;
    }
};

// Non-member operator<< calls virtual print()
std::ostream& operator<<(std::ostream& os, const GameEntity& entity) {
    os << entity.typeName() << " ";
    return entity.print(os);
}
```

**Key points:**
- First parameter: `std::ostream&`
- Second parameter: `const T&` (const reference for output)
- Return value: `std::ostream&` (enables chaining: `cout << a << b`)
- Use `friend` to access private members

## 2. Adding `operator>>` (Extractor)

To read into a custom type, overload `operator>>`:

```cpp
class GameEntity {
    friend std::istream& operator>>(std::istream& is, GameEntity& entity);
    
protected:
    virtual std::istream& input(std::istream& is) {
        is >> name_ >> x_ >> y_;
        return is;
    }
};

std::istream& operator>>(std::istream& is, GameEntity& entity) {
    return entity.input(is);
}
```

**Key points:**
- First parameter: `std::istream&`
- Second parameter: `T&` (non-const reference - we're modifying it!)
- Return value: `std::istream&` (enables chaining)

## 3. Polymorphic Serialization

For inheritance hierarchies, use **virtual functions**:

```cpp
class GameEntity {
public:
    virtual std::string typeName() const { return "GameEntity"; }
    
protected:
    virtual std::ostream& print(std::ostream& os) const {
        os << name_ << " " << x_ << " " << y_;
        return os;
    }
};

class Player : public GameEntity {
public:
    std::string typeName() const override { return "Player"; }
    
protected:
    std::ostream& print(std::ostream& os) const override {
        GameEntity::print(os);  // Base fields first
        os << " " << health_ << " " << level_;
        return os;
    }
};
```

Output:
```
Player Hero 10.5 20.3 100 5
Enemy Goblin 15.0 25.0 15 Aggressive
```

## 4. Polymorphic Deserialization (The Challenge)

Reading is harder because `operator>>` **doesn't know the derived type**:

```cpp
// Problem: What type should we create?
GameEntity entity;
cin >> entity;  // Always creates base class!
```

### Solution: Factory Pattern + Pointers

```cpp
// operator>> for unique_ptr creates the correct derived type
std::istream& operator>>(std::istream& is, std::unique_ptr<GameEntity>& ptr) {
    std::string typeName;
    is >> typeName;
    
    if (!is) return is;  // EOF
    
    // Factory creates correct derived type from string
    ptr = EntityFactory::instance().create(typeName);
    if (!ptr) {
        is.setstate(std::ios::failbit);
        return is;
    }
    
    ptr->input(is);  // Virtual call - reads derived fields!
    return is;
}
```

Usage:
```cpp
std::unique_ptr<GameEntity> entity;
cin >> entity;  // Reads "Player Hero 10 20 100 5"
                // Creates Player object, not GameEntity!
cout << *entity; // Outputs "Player Hero 10 20 100 5"
```

## 5. The Entity Factory

```cpp
class EntityFactory {
public:
    using CreatorFunc = std::function<std::unique_ptr<GameEntity>()>;

    static EntityFactory& instance() {
        static EntityFactory factory;
        return factory;
    }

    void enroll(const std::string& typeName, CreatorFunc creator) {
        creators_[typeName] = std::move(creator);
    }

    std::unique_ptr<GameEntity> create(const std::string& typeName) {
        auto it = creators_.find(typeName);
        if (it == creators_.end()) return nullptr;
        return it->second();
    }

private:
    std::map<std::string, CreatorFunc> creators_;
};
```

## 6. Self-Registration Pattern

Each class can register itself at static initialization:

```cpp
// Macro for self-registration
#define REGISTER_ENTITY(ClassName) \
    static bool ClassName##_registered = []() { \
        EntityFactory::instance().enroll(#ClassName, []() { \
            return std::make_unique<ClassName>(); \
        }); \
        return true; \
    }()

// In Player.cpp:
class Player : public GameEntity { /* ... */ };
REGISTER_ENTITY(Player);  // Registers before main() runs!
```

### ⚠️ Pros and Cons of Static Initialization

| Pros | Cons |
|------|------|
| Classes are self-contained | Order undefined across translation units |
| Adding new type = no changes elsewhere | Code runs before `main()` - hard to debug |
| Follows Open/Closed Principle | Linker may optimize away unused `.o` files |
| | Can cause subtle initialization bugs |

### Modern Alternatives

```cpp
// 1. Explicit registration (safer)
int main() {
    EntityFactory::enroll<Player>();
    EntityFactory::enroll<Enemy>();
    // ...
}

// 2. std::variant (compile-time, no heap)
using AnyEntity = std::variant<Player, Enemy, Collectible, Trigger>;

// 3. Reflection (C++26?)
```

## 7. Custom Manipulators

### Simple Manipulator (no arguments)

```cpp
// Like std::endl
std::ostream& debug_mode(std::ostream& os) {
    os << "[DEBUG] ";
    return os;
}

std::cout << debug_mode << "Starting game..." << std::endl;
// Output: [DEBUG] Starting game...
```

### Manipulator with Arguments

```cpp
struct EntityDebug {
    const GameEntity& entity;
};

std::ostream& operator<<(std::ostream& os, const EntityDebug& ed) {
    os << "[DEBUG] Type=" << ed.entity.typeName() 
       << " Name=\"" << ed.entity.getName() << "\"";
    return os;
}

EntityDebug debug(const GameEntity& e) { return EntityDebug(e); }

// Usage:
Player hero("Hero", 10, 20, 100, 5);
std::cout << debug(hero) << std::endl;
// Output: [DEBUG] Type=Player Name="Hero"
```

### Stateful Manipulator (using iword/pword)

```cpp
class CoordFormat {
public:
    enum Style { Compact, Verbose, JSON };
    
    static int xalloc_index() {
        static int index = std::ios_base::xalloc();
        return index;
    }
    
    static void setStyle(std::ostream& os, Style s) {
        os.iword(xalloc_index()) = static_cast<long>(s);
    }
    
    static Style getStyle(std::ostream& os) {
        return static_cast<Style>(os.iword(xalloc_index()));
    }
};

std::ostream& compact(std::ostream& os) {
    CoordFormat::setStyle(os, CoordFormat::Compact);
    return os;
}

// Usage:
Position pos(10.5, 20.3);
std::cout << compact << pos;  // (10.5,20.3)
std::cout << verbose << pos;  // x=10.5, y=20.3
std::cout << json_style << pos;  // {"x": 10.5, "y": 20.3}
```

## Real-World Applications

| Domain | Example |
|--------|---------|
| **Game Engines** | Save/load game state, entity serialization |
| **Plugin Systems** | Load modules by name (Photoshop filters, VSCode extensions) |
| **Serialization** | Protocol Buffers, Boost.Serialization, JSON/XML parsing |
| **GUI Frameworks** | Qt's meta-object system, Windows COM |
| **Dependency Injection** | Spring-like containers in C++ |

## Expected Output

```
=== 1. Adding operator<< (Inserter) ===
Serialized entities (using operator<<):
  Player Hero 10.5 20.3 100 5
  Enemy Goblin 15 25 15 Aggressive
  Collectible GoldCoin 12 22 50 Currency
  Trigger DoorTrigger 20 20 3 OpenDoor

=== 2. Adding operator>> (Extractor) ===
Loading entities from 'save file':
  Loaded: Player SavedHero 100 200 85 12
  Loaded: Enemy Dragon 500 300 50 Boss
  ...

=== 3. Factory Pattern for Polymorphic Deserialization ===
Registered entity types in factory:
  - Player
  - Enemy
  - Collectible
  - Trigger
```

## Summary

| Technique | Purpose |
|-----------|---------|
| `operator<<` | Serialize object to stream |
| `operator>>` | Deserialize object from stream |
| virtual `print()`/`input()` | Polymorphic serialization |
| Factory pattern | Create derived types from strings |
| Self-registration | Auto-enroll types (use with caution) |
| Custom manipulators | Format output (like `std::hex`) |

## Related Topics

- [06_TypeErasure](../06_TypeErasure/README.md) - Another polymorphism approach
- [01_RAII](../01_RAII/README.md) - Resource management for streams

## References

- [cppreference: Input/output library](https://en.cppreference.com/w/cpp/io)
- [cppreference: operator<< overloading](https://en.cppreference.com/w/cpp/language/operators)
- [cppreference: ios_base::xalloc](https://en.cppreference.com/w/cpp/io/ios_base/xalloc)

