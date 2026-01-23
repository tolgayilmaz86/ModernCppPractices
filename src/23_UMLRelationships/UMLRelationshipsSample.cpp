#include "UMLRelationshipsSample.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
namespace {

// ============================================================================
// 1. DEPENDENCY (Uses-a temporarily)
// ============================================================================
// UML Notation: - - - - - - - - - - - - - - >
// Dashed line with open arrow pointing to the dependency
//
// A class USES another class temporarily (e.g., as parameter, local variable)
// Weakest relationship - no permanent connection

class Printer {
public:
    void print(const std::string& message) {
        std::cout << "[PRINTER] " << message << std::endl;
    }
};

// Document DEPENDS on Printer (uses it temporarily in a method)
class Document {
private:
    std::string content_;

public:
    explicit Document(std::string content) : content_(std::move(content)) {}

    // Dependency: Printer is passed as parameter, used temporarily
    void printTo(Printer& printer) {
        printer.print(content_);
    }

    // Another form of dependency: creating local instance
    void printWithNewPrinter() {
        Printer localPrinter;  // Local dependency
        localPrinter.print(content_);
    }

    const std::string& content() const { return content_; }
};

void demonstrate_dependency() {
    std::cout << "\n=== 1. DEPENDENCY (Uses-a temporarily) ===" << std::endl;
    std::cout << "UML Notation: Document - - - -> Printer (dashed arrow)" << std::endl;
    std::cout << "Meaning: Document temporarily uses Printer\n" << std::endl;

    Document doc("Hello from Document!");
    Printer printer;

    std::cout << "Document uses Printer via method parameter:" << std::endl;
    doc.printTo(printer);

    std::cout << "\nDocument creates local Printer instance:" << std::endl;
    doc.printWithNewPrinter();

    std::cout << "\nCharacteristics:" << std::endl;
    std::cout << "- Weakest relationship" << std::endl;
    std::cout << "- No member variable storing the dependency" << std::endl;
    std::cout << "- Used as: parameter, local variable, or return type" << std::endl;
    std::cout << "- Changes to Printer may affect Document's methods" << std::endl;
}

// ============================================================================
// 2. ASSOCIATION (Knows-a, uses-a with reference)
// ============================================================================
// UML Notation: ────────────────────────────>
// Solid line with open arrow (or no arrow for bidirectional)
//
// A class KNOWS about another class (stores reference/pointer)
// Neither class owns the other - independent lifetimes

class Course;  // Forward declaration

class Student {
private:
    std::string name_;
    std::vector<Course*> enrolled_courses_;  // Association: knows about courses

public:
    explicit Student(std::string name) : name_(std::move(name)) {}

    void enrollIn(Course* course) {
        enrolled_courses_.push_back(course);
        std::cout << name_ << " enrolled in a course" << std::endl;
    }

    void listCourses() const;
    const std::string& name() const { return name_; }
};

class Course {
private:
    std::string title_;
    std::vector<Student*> students_;  // Association: knows about students

public:
    explicit Course(std::string title) : title_(std::move(title)) {}

    void addStudent(Student* student) {
        students_.push_back(student);
        student->enrollIn(this);
    }

    void listStudents() const {
        std::cout << "Course '" << title_ << "' has students: ";
        for (const auto* s : students_) {
            std::cout << s->name() << " ";
        }
        std::cout << std::endl;
    }

    const std::string& title() const { return title_; }
};

void Student::listCourses() const {
    std::cout << name_ << " is enrolled in: ";
    for (const auto* c : enrolled_courses_) {
        std::cout << c->title() << " ";
    }
    std::cout << std::endl;
}

void demonstrate_association() {
    std::cout << "\n=== 2. ASSOCIATION (Knows-a) ===" << std::endl;
    std::cout << "UML Notation: Student ───────> Course (solid line)" << std::endl;
    std::cout << "Meaning: Student knows about Course (and vice versa)\n" << std::endl;

    // Both have independent lifetimes
    Student alice("Alice");
    Student bob("Bob");
    Course cpp("C++ Programming");
    Course algorithms("Algorithms");

    cpp.addStudent(&alice);
    cpp.addStudent(&bob);
    algorithms.addStudent(&alice);

    cpp.listStudents();
    algorithms.listStudents();
    alice.listCourses();
    bob.listCourses();

    std::cout << "\nCharacteristics:" << std::endl;
    std::cout << "- Objects have independent lifetimes" << std::endl;
    std::cout << "- One class stores reference/pointer to another" << std::endl;
    std::cout << "- Can be unidirectional or bidirectional" << std::endl;
    std::cout << "- Deletion of one doesn't affect the other" << std::endl;
}

// ============================================================================
// 3. AGGREGATION (Has-a, weak ownership)
// ============================================================================
// UML Notation: ◇────────────────────────────
// Empty diamond on the aggregate side, line to the part
//
// A class HAS another class as a part, but doesn't own it
// Part can exist independently and be shared

class Engine {
private:
    std::string model_;
    int horsepower_;

public:
    Engine(std::string model, int hp) : model_(std::move(model)), horsepower_(hp) {
        std::cout << "Engine '" << model_ << "' created" << std::endl;
    }

    ~Engine() {
        std::cout << "Engine '" << model_ << "' destroyed" << std::endl;
    }

    void start() const {
        std::cout << "Engine '" << model_ << "' (" << horsepower_ << " HP) started" << std::endl;
    }

    const std::string& model() const { return model_; }
};

// Car AGGREGATES Engine (has-a, but doesn't own)
class Car {
private:
    std::string brand_;
    Engine* engine_;  // Aggregation: Car has engine but doesn't own it

public:
    Car(std::string brand, Engine* engine)
        : brand_(std::move(brand)), engine_(engine) {
        std::cout << "Car '" << brand_ << "' created with external engine" << std::endl;
    }

    ~Car() {
        std::cout << "Car '" << brand_ << "' destroyed (engine NOT destroyed)" << std::endl;
        // Note: We don't delete engine_ - we don't own it!
    }

    void start() const {
        std::cout << "Starting car '" << brand_ << "'..." << std::endl;
        if (engine_) engine_->start();
    }

    void setEngine(Engine* engine) { engine_ = engine; }
};

void demonstrate_aggregation() {
    std::cout << "\n=== 3. AGGREGATION (Has-a, weak ownership) ===" << std::endl;
    std::cout << "UML Notation: Car ◇─────── Engine (empty diamond)" << std::endl;
    std::cout << "Meaning: Car has Engine, but Engine can exist independently\n" << std::endl;

    // Engine exists independently of Car
    Engine v8("V8 Turbo", 450);

    {
        Car sportsCar("Ferrari", &v8);
        sportsCar.start();
        std::cout << "\n--- Sports car going out of scope ---" << std::endl;
    }  // Car destroyed, but engine survives!

    std::cout << "\nEngine still exists after car is destroyed:" << std::endl;
    v8.start();  // Engine still works!

    std::cout << "\nEngine can be shared between cars:" << std::endl;
    {
        Car car1("Car1", &v8);
        Car car2("Car2", &v8);  // Same engine, different cars
        car1.start();
        car2.start();
    }

    std::cout << "\nCharacteristics:" << std::endl;
    std::cout << "- Part can exist independently of the whole" << std::endl;
    std::cout << "- Part can be shared among multiple aggregates" << std::endl;
    std::cout << "- Aggregate doesn't manage part's lifetime" << std::endl;
    std::cout << "- Typically uses raw pointers or references" << std::endl;
}

// ============================================================================
// 4. COMPOSITION (Owns-a, strong ownership)
// ============================================================================
// UML Notation: ◆────────────────────────────
// Filled diamond on the composite side, line to the part
//
// A class OWNS another class - part cannot exist without whole
// Whole manages part's lifetime

class Heart {
private:
    int beats_per_minute_;

public:
    explicit Heart(int bpm = 70) : beats_per_minute_(bpm) {
        std::cout << "Heart created (BPM: " << beats_per_minute_ << ")" << std::endl;
    }

    ~Heart() {
        std::cout << "Heart destroyed" << std::endl;
    }

    void beat() const {
        std::cout << "Heart beating at " << beats_per_minute_ << " BPM" << std::endl;
    }
};

class Brain {
private:
    std::string thoughts_;

public:
    Brain() : thoughts_("thinking...") {
        std::cout << "Brain created" << std::endl;
    }

    ~Brain() {
        std::cout << "Brain destroyed" << std::endl;
    }

    void think(const std::string& thought) {
        thoughts_ = thought;
        std::cout << "Brain is thinking: " << thoughts_ << std::endl;
    }
};

// Human COMPOSES Heart and Brain (owns them completely)
class Human {
private:
    std::string name_;
    std::unique_ptr<Heart> heart_;   // Composition: Human owns Heart
    std::unique_ptr<Brain> brain_;   // Composition: Human owns Brain

public:
    explicit Human(std::string name)
        : name_(std::move(name)),
          heart_(std::make_unique<Heart>(72)),
          brain_(std::make_unique<Brain>()) {
        std::cout << "Human '" << name_ << "' created with heart and brain" << std::endl;
    }

    ~Human() {
        std::cout << "Human '" << name_ << "' destroyed (organs destroyed too)" << std::endl;
    }

    void live() const {
        std::cout << name_ << " is alive:" << std::endl;
        heart_->beat();
        brain_->think("I exist!");
    }
};

void demonstrate_composition() {
    std::cout << "\n=== 4. COMPOSITION (Owns-a, strong ownership) ===" << std::endl;
    std::cout << "UML Notation: Human ◆─────── Heart (filled diamond)" << std::endl;
    std::cout << "Meaning: Human owns Heart; Heart cannot exist without Human\n" << std::endl;

    {
        Human person("John");
        person.live();
        std::cout << "\n--- Person going out of scope ---" << std::endl;
    }  // Human destroyed, and so are Heart and Brain!

    std::cout << "\nCharacteristics:" << std::endl;
    std::cout << "- Part cannot exist without the whole" << std::endl;
    std::cout << "- Whole manages part's complete lifecycle" << std::endl;
    std::cout << "- Part is not shared between composites" << std::endl;
    std::cout << "- Typically uses std::unique_ptr or direct member" << std::endl;
}

// ============================================================================
// 5. INHERITANCE / GENERALIZATION (Is-a)
// ============================================================================
// UML Notation: ─────────────────────────────▷
// Solid line with hollow triangle pointing to the base class
//
// A class IS A specialized version of another class
// Child inherits behavior and attributes from parent

class Animal {
protected:
    std::string name_;
    int age_;

public:
    Animal(std::string name, int age) : name_(std::move(name)), age_(age) {
        std::cout << "Animal '" << name_ << "' created" << std::endl;
    }

    virtual ~Animal() {
        std::cout << "Animal '" << name_ << "' destroyed" << std::endl;
    }

    virtual void speak() const {
        std::cout << name_ << " makes a sound" << std::endl;
    }

    virtual void move() const {
        std::cout << name_ << " moves" << std::endl;
    }

    void sleep() const {
        std::cout << name_ << " is sleeping (age: " << age_ << ")" << std::endl;
    }
};

// Dog IS-A Animal (inheritance)
class Dog : public Animal {
private:
    std::string breed_;

public:
    Dog(std::string name, int age, std::string breed)
        : Animal(std::move(name), age), breed_(std::move(breed)) {
        std::cout << "Dog '" << name_ << "' (breed: " << breed_ << ") created" << std::endl;
    }

    ~Dog() override {
        std::cout << "Dog '" << name_ << "' destroyed" << std::endl;
    }

    void speak() const override {
        std::cout << name_ << " the " << breed_ << " barks: Woof!" << std::endl;
    }

    void move() const override {
        std::cout << name_ << " runs on four legs" << std::endl;
    }

    void fetch() const {
        std::cout << name_ << " fetches the ball!" << std::endl;
    }
};

// Cat IS-A Animal (inheritance)
class Cat : public Animal {
public:
    Cat(std::string name, int age) : Animal(std::move(name), age) {
        std::cout << "Cat '" << name_ << "' created" << std::endl;
    }

    ~Cat() override {
        std::cout << "Cat '" << name_ << "' destroyed" << std::endl;
    }

    void speak() const override {
        std::cout << name_ << " meows: Meow!" << std::endl;
    }

    void move() const override {
        std::cout << name_ << " walks gracefully" << std::endl;
    }
};

void demonstrate_inheritance() {
    std::cout << "\n=== 5. INHERITANCE / GENERALIZATION (Is-a) ===" << std::endl;
    std::cout << "UML Notation: Dog ─────────▷ Animal (hollow triangle)" << std::endl;
    std::cout << "Meaning: Dog IS-A Animal (specialization)\n" << std::endl;

    std::vector<std::unique_ptr<Animal>> animals;
    animals.push_back(std::make_unique<Dog>("Buddy", 3, "Golden Retriever"));
    animals.push_back(std::make_unique<Cat>("Whiskers", 5));

    std::cout << "\nPolymorphic behavior:" << std::endl;
    for (const auto& animal : animals) {
        animal->speak();
        animal->move();
        animal->sleep();  // Inherited behavior
        std::cout << std::endl;
    }

    // Dog-specific behavior
    Dog rex("Rex", 2, "German Shepherd");
    rex.fetch();

    std::cout << "\nCharacteristics:" << std::endl;
    std::cout << "- Child inherits parent's attributes and methods" << std::endl;
    std::cout << "- Child can override virtual methods" << std::endl;
    std::cout << "- Child can add new attributes and methods" << std::endl;
    std::cout << "- Enables polymorphism through base class pointers" << std::endl;
}

// ============================================================================
// 6. REALIZATION / IMPLEMENTATION (Implements interface)
// ============================================================================
// UML Notation: - - - - - - - - - - - - - - -▷
// Dashed line with hollow triangle pointing to the interface
//
// A class IMPLEMENTS an interface (abstract class in C++)
// Class provides concrete implementation of interface methods

// Interface (pure abstract class)
class IDrawable {
public:
    virtual ~IDrawable() = default;
    virtual void draw() const = 0;
    virtual void resize(double factor) = 0;
};

// Interface for serialization
class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& data) = 0;
};

// Circle REALIZES IDrawable and ISerializable
class Circle : public IDrawable, public ISerializable {
private:
    double radius_;
    std::string color_;

public:
    Circle(double radius, std::string color)
        : radius_(radius), color_(std::move(color)) {
        std::cout << "Circle created (radius: " << radius_ << ", color: " << color_ << ")" << std::endl;
    }

    // IDrawable implementation
    void draw() const override {
        std::cout << "Drawing " << color_ << " circle with radius " << radius_ << std::endl;
    }

    void resize(double factor) override {
        radius_ *= factor;
        std::cout << "Circle resized to radius " << radius_ << std::endl;
    }

    // ISerializable implementation
    std::string serialize() const override {
        return "Circle:" + std::to_string(radius_) + ":" + color_;
    }

    void deserialize(const std::string& data) override {
        std::cout << "Deserializing circle from: " << data << std::endl;
        // In real code, parse the data
    }
};

// Rectangle REALIZES IDrawable
class Rectangle : public IDrawable {
private:
    double width_;
    double height_;

public:
    Rectangle(double width, double height) : width_(width), height_(height) {
        std::cout << "Rectangle created (" << width_ << "x" << height_ << ")" << std::endl;
    }

    void draw() const override {
        std::cout << "Drawing rectangle " << width_ << "x" << height_ << std::endl;
    }

    void resize(double factor) override {
        width_ *= factor;
        height_ *= factor;
        std::cout << "Rectangle resized to " << width_ << "x" << height_ << std::endl;
    }
};

void demonstrate_realization() {
    std::cout << "\n=== 6. REALIZATION / IMPLEMENTATION (Implements) ===" << std::endl;
    std::cout << "UML Notation: Circle - - - -▷ IDrawable (dashed + hollow triangle)" << std::endl;
    std::cout << "Meaning: Circle implements IDrawable interface\n" << std::endl;

    std::vector<std::unique_ptr<IDrawable>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0, "red"));
    shapes.push_back(std::make_unique<Rectangle>(10.0, 20.0));

    std::cout << "\nDrawing all shapes (polymorphism via interface):" << std::endl;
    for (const auto& shape : shapes) {
        shape->draw();
        shape->resize(1.5);
    }

    std::cout << "\nCircle also implements ISerializable:" << std::endl;
    Circle circle(3.0, "blue");
    ISerializable* serializable = &circle;
    std::cout << "Serialized: " << serializable->serialize() << std::endl;

    std::cout << "\nCharacteristics:" << std::endl;
    std::cout << "- Interface defines contract (pure virtual methods)" << std::endl;
    std::cout << "- Class provides concrete implementation" << std::endl;
    std::cout << "- A class can implement multiple interfaces" << std::endl;
    std::cout << "- Enables programming to interfaces, not implementations" << std::endl;
}

// ============================================================================
// Comprehensive Example: All Relationships Together
// ============================================================================

// Forward declarations
class OrderItem;
class PaymentProcessor;
class Customer;
class Order;

// Interface for notification
class INotifiable {
public:
    virtual ~INotifiable() = default;
    virtual void notify(const std::string& message) = 0;
};

// Concrete notification via email
class EmailNotifier : public INotifiable {
public:
    void notify(const std::string& message) override {
        std::cout << "[EMAIL] " << message << std::endl;
    }
};

// Product class (independent entity)
class Product {
private:
    std::string name_;
    double price_;

public:
    Product(std::string name, double price) : name_(std::move(name)), price_(price) {}
    const std::string& name() const { return name_; }
    double price() const { return price_; }
};

// OrderItem - Composed by Order (cannot exist without Order)
class OrderItem {
private:
    Product* product_;  // Association: knows about product
    int quantity_;

public:
    OrderItem(Product* product, int quantity)
        : product_(product), quantity_(quantity) {}

    double total() const { return product_->price() * quantity_; }
    std::string description() const {
        return std::to_string(quantity_) + "x " + product_->name();
    }
};

// PaymentProcessor - Used by Order (Dependency)
class PaymentProcessor {
public:
    bool processPayment(double amount) {
        std::cout << "Processing payment of $" << amount << std::endl;
        return true;  // Simulate success
    }
};

// Customer class (can exist independently)
class Customer : public INotifiable {  // Realization
private:
    std::string name_;
    std::string email_;

public:
    Customer(std::string name, std::string email)
        : name_(std::move(name)), email_(std::move(email)) {}

    const std::string& name() const { return name_; }

    void notify(const std::string& message) override {
        std::cout << "[TO: " << email_ << "] " << message << std::endl;
    }
};

// Order class demonstrating multiple relationships
class Order {
private:
    int order_id_;
    Customer* customer_;                           // Association: knows customer
    std::vector<std::unique_ptr<OrderItem>> items_; // Composition: owns items

public:
    Order(int id, Customer* customer) : order_id_(id), customer_(customer) {
        std::cout << "Order #" << order_id_ << " created for " << customer_->name() << std::endl;
    }

    ~Order() {
        std::cout << "Order #" << order_id_ << " destroyed (items destroyed too)" << std::endl;
    }

    void addItem(Product* product, int quantity) {
        items_.push_back(std::make_unique<OrderItem>(product, quantity));
    }

    double total() const {
        double sum = 0;
        for (const auto& item : items_) {
            sum += item->total();
        }
        return sum;
    }

    // Dependency: uses PaymentProcessor temporarily
    bool checkout(PaymentProcessor& processor) {
        std::cout << "\nChecking out Order #" << order_id_ << ":" << std::endl;
        for (const auto& item : items_) {
            std::cout << "  - " << item->description() << std::endl;
        }
        std::cout << "  Total: $" << total() << std::endl;

        if (processor.processPayment(total())) {
            customer_->notify("Your order #" + std::to_string(order_id_) + " has been placed!");
            return true;
        }
        return false;
    }
};

void demonstrate_comprehensive_example() {
    std::cout << "\n=== Comprehensive Example: E-Commerce System ===" << std::endl;
    std::cout << "Demonstrating all UML relationships together\n" << std::endl;

    std::cout << "Relationships in this example:" << std::endl;
    std::cout << "- Order ◆───── OrderItem    (Composition)" << std::endl;
    std::cout << "- Order ───────> Customer   (Association)" << std::endl;
    std::cout << "- Order - - -> PaymentProcessor (Dependency)" << std::endl;
    std::cout << "- OrderItem ──> Product     (Association)" << std::endl;
    std::cout << "- Customer - -▷ INotifiable (Realization)" << std::endl;
    std::cout << std::endl;

    // Products exist independently
    Product laptop("Laptop", 999.99);
    Product mouse("Mouse", 29.99);
    Product keyboard("Keyboard", 79.99);

    // Customer exists independently
    Customer customer("Alice", "alice@example.com");

    // PaymentProcessor is used temporarily
    PaymentProcessor processor;

    {
        // Order is created and owns OrderItems
        Order order(1001, &customer);
        order.addItem(&laptop, 1);
        order.addItem(&mouse, 2);
        order.addItem(&keyboard, 1);

        order.checkout(processor);

        std::cout << "\n--- Order going out of scope ---" << std::endl;
    }

    std::cout << "\nProducts and Customer still exist after Order is destroyed" << std::endl;
    std::cout << "Customer: " << customer.name() << std::endl;
    std::cout << "Product: " << laptop.name() << " - $" << laptop.price() << std::endl;
}

void print_uml_summary() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "               UML RELATIONSHIP SUMMARY" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    std::cout << R"(
┌─────────────────────────────────────────────────────────────────────┐
│  Relationship    │  UML Notation           │  C++ Implementation   │
├─────────────────────────────────────────────────────────────────────┤
│  Dependency      │  A - - - -> B           │  Parameter, local var │
│  (uses)          │  (dashed arrow)         │  or return type       │
├─────────────────────────────────────────────────────────────────────┤
│  Association     │  A ─────────> B         │  Member pointer/ref   │
│  (knows)         │  (solid arrow)          │  (no ownership)       │
├─────────────────────────────────────────────────────────────────────┤
│  Aggregation     │  A ◇──────── B          │  Member pointer/ref   │
│  (has, shared)   │  (empty diamond)        │  (shared, no delete)  │
├─────────────────────────────────────────────────────────────────────┤
│  Composition     │  A ◆──────── B          │  unique_ptr or        │
│  (owns)          │  (filled diamond)       │  direct member        │
├─────────────────────────────────────────────────────────────────────┤
│  Inheritance     │  A ──────────▷ B        │  class A : public B   │
│  (is-a)          │  (hollow triangle)      │  (virtual methods)    │
├─────────────────────────────────────────────────────────────────────┤
│  Realization     │  A - - - - - -▷ B       │  class A : public IB  │
│  (implements)    │  (dashed + triangle)    │  (pure virtual)       │
└─────────────────────────────────────────────────────────────────────┘

Strength of Relationships (weakest to strongest):
  Dependency < Association < Aggregation < Composition < Inheritance

Key Decision Points:
  • Does B exist without A?  Yes → Aggregation/Association
                             No → Composition
  • Does A own B's lifetime? Yes → Composition (unique_ptr)
                             No → Aggregation (raw ptr/shared_ptr)
  • Is A a type of B?        Yes → Inheritance
  • Does A implement B's contract? Yes → Realization (interface)
)" << std::endl;
}

} // end anonymous namespace

#include "../SampleRegistry.hpp"

void UMLRelationshipsSample::run() {
    std::cout << "Running UML Relationships Sample..." << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Demonstrating how UML class diagram relationships" << std::endl;
    std::cout << "translate to C++ code." << std::endl;

    demonstrate_dependency();
    demonstrate_association();
    demonstrate_aggregation();
    demonstrate_composition();
    demonstrate_inheritance();
    demonstrate_realization();
    demonstrate_comprehensive_example();
    print_uml_summary();

    std::cout << "\nUML Relationships demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(UMLRelationshipsSample, "UML Relationships", 23);
