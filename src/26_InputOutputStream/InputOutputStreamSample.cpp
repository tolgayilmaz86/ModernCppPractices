#include "InputOutputStreamSample.hpp"
#include "SampleRegistry.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include <iomanip>

// ============================================================================
// GAME ENTITY HIERARCHY - Base class for all game objects
// ============================================================================

namespace GameEngine {

// Forward declarations
class GameEntity;
class EntityFactory;

// ============================================================================
// BASE CLASS: GameEntity
// ============================================================================

class GameEntity {
public:
    GameEntity() = default;
    GameEntity(std::string name, float x, float y) 
        : name_(std::move(name)), x_(x), y_(y) {}
    
    virtual ~GameEntity() = default;

    // Getters
    const std::string& getName() const { return name_; }
    float getX() const { return x_; }
    float getY() const { return y_; }

    // Virtual type identifier for serialization
    virtual std::string typeName() const { return "GameEntity"; }

    // Friend declaration for operator<< (calls virtual print)
    friend std::ostream& operator<<(std::ostream& os, const GameEntity& entity);
    
    // Friend declaration for operator>> (calls virtual input)
    friend std::istream& operator>>(std::istream& is, GameEntity& entity);

    // Virtual print - derived classes override to add their fields
    virtual std::ostream& print(std::ostream& os) const {
        os << name_ << " " << x_ << " " << y_;
        return os;
    }

    // Virtual input - derived classes override to read their fields
    // Public so operator>> for unique_ptr can access it
    virtual std::istream& input(std::istream& is) {
        is >> name_ >> x_ >> y_;
        return is;
    }

protected:
    std::string name_ = "Unknown";
    float x_ = 0.0f;
    float y_ = 0.0f;
};

// ============================================================================
// ENTITY FACTORY - Creates entities from type strings (for deserialization)
// Must be defined AFTER GameEntity is complete (unique_ptr needs complete type)
// ============================================================================

class EntityFactory {
public:
    using CreatorFunc = std::function<std::unique_ptr<GameEntity>()>;

    static EntityFactory& instance() {
        static EntityFactory factory;
        return factory;
    }

    void enroll(const std::string& typeName, CreatorFunc creator) {
        creators_[typeName] = std::move(creator);
        std::cout << "  [Factory] Enrolled: " << typeName << std::endl;
    }

    std::unique_ptr<GameEntity> create(const std::string& typeName) {
        auto it = creators_.find(typeName);
        if (it == creators_.end()) {
            return nullptr;  // Unknown type
        }
        return it->second();
    }

    bool isRegistered(const std::string& typeName) const {
        return creators_.find(typeName) != creators_.end();
    }

    std::vector<std::string> registeredTypes() const {
        std::vector<std::string> types;
        for (const auto& [name, _] : creators_) {
            types.push_back(name);
        }
        return types;
    }

private:
    std::map<std::string, CreatorFunc> creators_;
    EntityFactory() = default;
};

// Helper macro for self-registration (demonstrates the pattern)
#define REGISTER_ENTITY(ClassName) \
    static bool ClassName##_registered = []() { \
        EntityFactory::instance().enroll(#ClassName, []() { \
            return std::make_unique<ClassName>(); \
        }); \
        return true; \
    }()

// Non-member operator<< calls virtual print()
inline std::ostream& operator<<(std::ostream& os, const GameEntity& entity) {
    os << entity.typeName() << " ";
    return entity.print(os);
}

// Non-member operator>> for existing objects
inline std::istream& operator>>(std::istream& is, GameEntity& entity) {
    return entity.input(is);
}

// ============================================================================
// POLYMORPHIC DESERIALIZATION - Read type string, create correct object
// ============================================================================

// Operator>> for pointer - creates the right derived type!
inline std::istream& operator>>(std::istream& is, std::unique_ptr<GameEntity>& entityPtr) {
    std::string typeName;
    is >> typeName;
    
    if (!is) return is;  // EOF or error
    
    entityPtr = EntityFactory::instance().create(typeName);
    if (!entityPtr) {
        is.setstate(std::ios::failbit);  // Unknown type
        std::cerr << "Unknown entity type: " << typeName << std::endl;
        return is;
    }
    
    // Call virtual input() on the correctly-typed object
    entityPtr->input(is);
    return is;
}

// ============================================================================
// DERIVED CLASS: Player
// ============================================================================

class Player : public GameEntity {
public:
    Player() = default;
    Player(std::string name, float x, float y, int health, int level)
        : GameEntity(std::move(name), x, y), health_(health), level_(level) {}

    std::string typeName() const override { return "Player"; }
    
    int getHealth() const { return health_; }
    int getLevel() const { return level_; }

protected:
    std::ostream& print(std::ostream& os) const override {
        GameEntity::print(os);  // Print base fields first
        os << " " << health_ << " " << level_;
        return os;
    }

    std::istream& input(std::istream& is) override {
        GameEntity::input(is);  // Read base fields first
        is >> health_ >> level_;
        return is;
    }

private:
    int health_ = 100;
    int level_ = 1;
};

// Self-registration
REGISTER_ENTITY(Player);

// ============================================================================
// DERIVED CLASS: Enemy
// ============================================================================

class Enemy : public GameEntity {
public:
    Enemy() = default;
    Enemy(std::string name, float x, float y, int damage, std::string aiType)
        : GameEntity(std::move(name), x, y), damage_(damage), aiType_(std::move(aiType)) {}

    std::string typeName() const override { return "Enemy"; }
    
    int getDamage() const { return damage_; }
    const std::string& getAIType() const { return aiType_; }

protected:
    std::ostream& print(std::ostream& os) const override {
        GameEntity::print(os);
        os << " " << damage_ << " " << aiType_;
        return os;
    }

    std::istream& input(std::istream& is) override {
        GameEntity::input(is);
        is >> damage_ >> aiType_;
        return is;
    }

private:
    int damage_ = 10;
    std::string aiType_ = "Patrol";
};

REGISTER_ENTITY(Enemy);

// ============================================================================
// DERIVED CLASS: Collectible (Item)
// ============================================================================

class Collectible : public GameEntity {
public:
    Collectible() = default;
    Collectible(std::string name, float x, float y, int value, std::string itemType)
        : GameEntity(std::move(name), x, y), value_(value), itemType_(std::move(itemType)) {}

    std::string typeName() const override { return "Collectible"; }
    
    int getValue() const { return value_; }
    const std::string& getItemType() const { return itemType_; }

protected:
    std::ostream& print(std::ostream& os) const override {
        GameEntity::print(os);
        os << " " << value_ << " " << itemType_;
        return os;
    }

    std::istream& input(std::istream& is) override {
        GameEntity::input(is);
        is >> value_ >> itemType_;
        return is;
    }

private:
    int value_ = 0;
    std::string itemType_ = "Coin";
};

REGISTER_ENTITY(Collectible);

// ============================================================================
// DERIVED CLASS: Trigger (invisible game logic zones)
// ============================================================================

class Trigger : public GameEntity {
public:
    Trigger() = default;
    Trigger(std::string name, float x, float y, float radius, std::string eventName)
        : GameEntity(std::move(name), x, y), radius_(radius), eventName_(std::move(eventName)) {}

    std::string typeName() const override { return "Trigger"; }
    
    float getRadius() const { return radius_; }
    const std::string& getEventName() const { return eventName_; }

protected:
    std::ostream& print(std::ostream& os) const override {
        GameEntity::print(os);
        os << " " << radius_ << " " << eventName_;
        return os;
    }

    std::istream& input(std::istream& is) override {
        GameEntity::input(is);
        is >> radius_ >> eventName_;
        return is;
    }

private:
    float radius_ = 1.0f;
    std::string eventName_ = "OnEnter";
};

REGISTER_ENTITY(Trigger);

} // namespace GameEngine

// ============================================================================
// CUSTOM MANIPULATOR EXAMPLE
// ============================================================================

namespace Manipulators {

// Custom manipulator: prints entity in a "debug" format
struct EntityDebug {
    const GameEngine::GameEntity& entity;
    explicit EntityDebug(const GameEngine::GameEntity& e) : entity(e) {}
};

inline std::ostream& operator<<(std::ostream& os, const EntityDebug& ed) {
    os << "[DEBUG] Type=" << ed.entity.typeName() 
       << " Name=\"" << ed.entity.getName() << "\""
       << " Pos=(" << std::fixed << std::setprecision(1) 
       << ed.entity.getX() << ", " << ed.entity.getY() << ")";
    return os;
}

inline EntityDebug debug(const GameEngine::GameEntity& e) {
    return EntityDebug(e);
}

// Custom manipulator with state: coordinate format
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

// Manipulators to set coordinate format
inline std::ostream& compact(std::ostream& os) {
    CoordFormat::setStyle(os, CoordFormat::Compact);
    return os;
}

inline std::ostream& verbose(std::ostream& os) {
    CoordFormat::setStyle(os, CoordFormat::Verbose);
    return os;
}

inline std::ostream& json_style(std::ostream& os) {
    CoordFormat::setStyle(os, CoordFormat::JSON);
    return os;
}

// Position wrapper that respects the manipulator
struct Position {
    float x, y;
    Position(float x_, float y_) : x(x_), y(y_) {}
};

inline std::ostream& operator<<(std::ostream& os, const Position& p) {
    switch (CoordFormat::getStyle(os)) {
        case CoordFormat::Verbose:
            os << "x=" << p.x << ", y=" << p.y;
            break;
        case CoordFormat::JSON:
            os << "{\"x\": " << p.x << ", \"y\": " << p.y << "}";
            break;
        case CoordFormat::Compact:
        default:
            os << "(" << p.x << "," << p.y << ")";
            break;
    }
    return os;
}

} // namespace Manipulators

// ============================================================================
// DEMONSTRATION
// ============================================================================

void InputOutputStreamSample::run() {
    using namespace GameEngine;
    using namespace Manipulators;

    std::cout << "\n=== 1. Adding operator<< (Inserter) ===" << std::endl;
    {
        Player hero("Hero", 10.5f, 20.3f, 100, 5);
        Enemy goblin("Goblin", 15.0f, 25.0f, 15, "Aggressive");
        Collectible coin("GoldCoin", 12.0f, 22.0f, 50, "Currency");
        Trigger door("DoorTrigger", 20.0f, 20.0f, 3.0f, "OpenDoor");

        std::cout << "Serialized entities (using operator<<):" << std::endl;
        std::cout << "  " << hero << std::endl;
        std::cout << "  " << goblin << std::endl;
        std::cout << "  " << coin << std::endl;
        std::cout << "  " << door << std::endl;

        std::cout << "\nPolymorphic output (base class reference):" << std::endl;
        std::vector<std::reference_wrapper<GameEntity>> entities = {hero, goblin, coin, door};
        for (const GameEntity& e : entities) {
            std::cout << "  " << e << std::endl;  // Calls correct virtual print()!
        }
    }

    std::cout << "\n=== 2. Adding operator>> (Extractor) ===" << std::endl;
    {
        // Simulate reading from a save file
        std::string saveData = 
            "Player SavedHero 100.0 200.0 85 12\n"
            "Enemy Dragon 500.0 300.0 50 Boss\n"
            "Collectible HealthPotion 105.0 205.0 100 Healing\n"
            "Trigger Checkpoint 150.0 250.0 5.0 SaveGame\n";

        std::cout << "Loading entities from 'save file':" << std::endl;
        std::cout << "Input data:\n" << saveData << std::endl;

        std::istringstream iss(saveData);
        std::vector<std::unique_ptr<GameEntity>> loadedEntities;

        std::unique_ptr<GameEntity> entity;
        while (iss >> entity) {  // Polymorphic deserialization!
            std::cout << "  Loaded: " << *entity << std::endl;
            loadedEntities.push_back(std::move(entity));
        }

        std::cout << "\nLoaded " << loadedEntities.size() << " entities total." << std::endl;
    }

    std::cout << "\n=== 3. Factory Pattern for Polymorphic Deserialization ===" << std::endl;
    {
        std::cout << "Registered entity types in factory:" << std::endl;
        for (const auto& type : EntityFactory::instance().registeredTypes()) {
            std::cout << "  - " << type << std::endl;
        }

        std::cout << "\nFactory creates correct derived type from string:" << std::endl;
        auto entity1 = EntityFactory::instance().create("Player");
        auto entity2 = EntityFactory::instance().create("Enemy");
        
        if (entity1) std::cout << "  Created: " << entity1->typeName() << std::endl;
        if (entity2) std::cout << "  Created: " << entity2->typeName() << std::endl;

        auto unknown = EntityFactory::instance().create("Dragon");
        std::cout << "  'Dragon' registered? " << (unknown ? "Yes" : "No") << std::endl;
    }

    std::cout << "\n=== 4. Custom Manipulators ===" << std::endl;
    {
        Player hero("TestPlayer", 123.456f, 789.012f, 100, 10);
        
        std::cout << "Debug manipulator:" << std::endl;
        std::cout << "  " << debug(hero) << std::endl;

        std::cout << "\nCoordinate format manipulators:" << std::endl;
        Position pos(123.456f, 789.012f);
        
        std::cout << compact << "  Compact: " << pos << std::endl;
        std::cout << verbose << "  Verbose: " << pos << std::endl;
        std::cout << json_style << "  JSON:    " << pos << std::endl;
    }

    std::cout << "\n=== 5. Round-Trip Serialization ===" << std::endl;
    {
        // Create original entities
        std::vector<std::unique_ptr<GameEntity>> original;
        original.push_back(std::make_unique<Player>("Alice", 10, 20, 100, 5));
        original.push_back(std::make_unique<Enemy>("Orc", 30, 40, 25, "Patrol"));
        original.push_back(std::make_unique<Collectible>("Gem", 50, 60, 500, "Treasure"));

        // Serialize to string
        std::ostringstream oss;
        for (const auto& e : original) {
            oss << *e << "\n";
        }
        std::string serialized = oss.str();

        std::cout << "Original entities serialized:" << std::endl;
        std::cout << serialized;

        // Deserialize back
        std::istringstream iss(serialized);
        std::vector<std::unique_ptr<GameEntity>> restored;
        
        std::unique_ptr<GameEntity> entity;
        while (iss >> entity) {
            restored.push_back(std::move(entity));
        }

        std::cout << "Restored entities:" << std::endl;
        for (const auto& e : restored) {
            std::cout << "  Type: " << e->typeName() 
                      << ", Name: " << e->getName() << std::endl;
        }

        std::cout << "\nRound-trip successful: " 
                  << (original.size() == restored.size() ? "Yes" : "No") << std::endl;
    }

    std::cout << "\n=== 6. Self-Registration Pattern (Static Initialization) ===" << std::endl;
    {
        std::cout << "How REGISTER_ENTITY macro works:" << std::endl;
        std::cout << R"(
  // This macro expands to:
  static bool Player_registered = []() {
      EntityFactory::instance().enroll("Player", []() {
          return std::make_unique<Player>();
      });
      return true;
  }();

  // The lambda runs at static initialization time,
  // BEFORE main() is called!
)" << std::endl;

        std::cout << "Pros:" << std::endl;
        std::cout << "  + Classes are self-contained" << std::endl;
        std::cout << "  + Adding new type requires no changes elsewhere" << std::endl;
        std::cout << "  + Follows Open/Closed Principle" << std::endl;
        
        std::cout << "\nCons:" << std::endl;
        std::cout << "  - Static initialization order is undefined across TUs" << std::endl;
        std::cout << "  - Code runs before main() - hard to debug" << std::endl;
        std::cout << "  - Linker may optimize away unused .o files" << std::endl;
        
        std::cout << "\nModern alternatives:" << std::endl;
        std::cout << "  - Explicit registration in main()" << std::endl;
        std::cout << "  - std::variant for compile-time polymorphism" << std::endl;
        std::cout << "  - Reflection (C++26?)" << std::endl;
    }

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "+---------------------------+----------------------------------------+" << std::endl;
    std::cout << "| Technique                 | Purpose                                |" << std::endl;
    std::cout << "+---------------------------+----------------------------------------+" << std::endl;
    std::cout << "| operator<<                | Serialize object to stream             |" << std::endl;
    std::cout << "| operator>>                | Deserialize object from stream         |" << std::endl;
    std::cout << "| virtual print()/input()   | Polymorphic serialization              |" << std::endl;
    std::cout << "| Factory pattern           | Create derived types from strings      |" << std::endl;
    std::cout << "| Self-registration         | Auto-enroll types (use with caution)   |" << std::endl;
    std::cout << "| Custom manipulators       | Format output (like std::hex)          |" << std::endl;
    std::cout << "+---------------------------+----------------------------------------+" << std::endl;
}

// Register this sample
REGISTER_SAMPLE(InputOutputStreamSample, "Input/Output Stream Extensions", 26);
