#include "PimplSample.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
namespace {

// Forward declaration of the implementation class
class PimplWidgetImpl;

// Public interface class using Pimpl
class PimplWidget {
private:
    std::unique_ptr<PimplWidgetImpl> pimpl_;  // Pointer to implementation

public:
    // Constructor
    PimplWidget(const std::string& name, int value);

    // Destructor
    ~PimplWidget();

    // Copy constructor
    PimplWidget(const PimplWidget& other);

    // Copy assignment
    PimplWidget& operator=(const PimplWidget& other);

    // Move constructor
    PimplWidget(PimplWidget&& other) noexcept;

    // Move assignment
    PimplWidget& operator=(PimplWidget&& other) noexcept;

    // Public interface methods
    void setName(const std::string& name);
    std::string getName() const;

    void setValue(int value);
    int getValue() const;

    void addData(int data);
    void printData() const;

    // Method that demonstrates implementation changes don't affect interface
    void processData();
};

// Implementation class (defined in .cpp file)
class PimplWidgetImpl {
private:
    std::string name_;
    int value_;
    std::vector<int> data_;

public:
    PimplWidgetImpl(const std::string& name, int value)
        : name_(name), value_(value) {}

    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }

    void setValue(int value) { value_ = value; }
    int getValue() const { return value_; }

    void addData(int data) { data_.push_back(data); }
    void printData() const {
        std::cout << "Data: [";
        for (size_t i = 0; i < data_.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data_[i];
        }
        std::cout << "]" << std::endl;
    }

    void processData() {
        // Simulate some complex processing that might change implementation
        for (auto& item : data_) {
            item *= value_;  // Multiply each data item by the value
        }
        std::cout << "Data processed (multiplied by " << value_ << ")" << std::endl;
    }
};

// PimplWidget implementation
PimplWidget::PimplWidget(const std::string& name, int value)
    : pimpl_(std::make_unique<PimplWidgetImpl>(name, value)) {}

PimplWidget::~PimplWidget() = default;  // unique_ptr handles deletion

PimplWidget::PimplWidget(const PimplWidget& other)
    : pimpl_(std::make_unique<PimplWidgetImpl>(*other.pimpl_)) {}

PimplWidget& PimplWidget::operator=(const PimplWidget& other) {
    if (this != &other) {
        pimpl_ = std::make_unique<PimplWidgetImpl>(*other.pimpl_);
    }
    return *this;
}

PimplWidget::PimplWidget(PimplWidget&& other) noexcept
    : pimpl_(std::move(other.pimpl_)) {
    // Ensure moved-from object is in a valid state
    other.pimpl_ = std::make_unique<PimplWidgetImpl>("", 0);
}

PimplWidget& PimplWidget::operator=(PimplWidget&& other) noexcept {
    if (this != &other) {
        pimpl_ = std::move(other.pimpl_);
        other.pimpl_ = std::make_unique<PimplWidgetImpl>("", 0);
    }
    return *this;
}

void PimplWidget::setName(const std::string& name) { pimpl_->setName(name); }
std::string PimplWidget::getName() const { return pimpl_->getName(); }

void PimplWidget::setValue(int value) { pimpl_->setValue(value); }
int PimplWidget::getValue() const { return pimpl_->getValue(); }

void PimplWidget::addData(int data) { pimpl_->addData(data); }
void PimplWidget::printData() const { pimpl_->printData(); }

void PimplWidget::processData() { pimpl_->processData(); }

// Example of a class that could benefit from Pimpl
// (Imagine this includes many heavy headers like <boost/...>, <Qt/...>, etc.)
class HeavyClass {
private:
    std::string data_;
    int counter_;

public:
    HeavyClass() : counter_(0) {}
    void setData(const std::string& data) { data_ = data; }
    std::string getData() const { return data_; }
    void increment() { ++counter_; }
    int getCounter() const { return counter_; }
};

// Public interface using Pimpl for the heavy class
class PublicInterface {
private:
    std::unique_ptr<HeavyClass> pimpl_;

public:
    PublicInterface() : pimpl_(std::make_unique<HeavyClass>()) {}
    ~PublicInterface() = default;

    // Copy operations
    PublicInterface(const PublicInterface& other)
        : pimpl_(std::make_unique<HeavyClass>(*other.pimpl_)) {}
    PublicInterface& operator=(const PublicInterface& other) {
        if (this != &other) {
            pimpl_ = std::make_unique<HeavyClass>(*other.pimpl_);
        }
        return *this;
    }

    // Public interface - clients don't need to know about HeavyClass
    void setData(const std::string& data) { pimpl_->setData(data); }
    std::string getData() const { return pimpl_->getData(); }
    void increment() { pimpl_->increment(); }
    int getCounter() const { return pimpl_->getCounter(); }
};

} // end anonymous namespace

#include "SampleRegistry.hpp"

void PimplSample::run() {
    std::cout << "Running Pimpl Sample..." << std::endl;

    // Basic Pimpl usage
    std::cout << "\n=== Basic Pimpl Usage ===" << std::endl;
    PimplWidget widget("MyWidget", 5);
    std::cout << "Created widget: " << widget.getName() << " with value " << widget.getValue() << std::endl;

    widget.addData(10);
    widget.addData(20);
    widget.addData(30);
    std::cout << "Added data to widget:" << std::endl;
    widget.printData();

    widget.processData();
    widget.printData();

    // Copy semantics
    std::cout << "\n=== Copy Semantics ===" << std::endl;
    PimplWidget widget2 = widget;  // Copy constructor
    std::cout << "Copied widget: " << widget2.getName() << " with value " << widget2.getValue() << std::endl;
    widget2.printData();

    widget2.setName("CopiedWidget");
    widget2.setValue(10);
    std::cout << "Modified copy: " << widget2.getName() << " with value " << widget2.getValue() << std::endl;

    // Original should be unchanged
    std::cout << "Original: " << widget.getName() << " with value " << widget.getValue() << std::endl;

    // Move semantics
    std::cout << "\n=== Move Semantics ===" << std::endl;
    PimplWidget widget3 = std::move(widget2);  // Move constructor
    std::cout << "Moved widget: " << widget3.getName() << " with value " << widget3.getValue() << std::endl;
    widget3.printData();

    // widget2 should be in valid but unspecified state
    std::cout << "Source after move: " << widget2.getName() << " with value " << widget2.getValue() << std::endl;

    // Public interface example
    std::cout << "\n=== Public Interface Example ===" << std::endl;
    PublicInterface interface;
    interface.setData("Hello Pimpl!");
    interface.increment();
    interface.increment();

    std::cout << "Data: " << interface.getData() << std::endl;
    std::cout << "Counter: " << interface.getCounter() << std::endl;

    // Copy the interface
    PublicInterface interface2 = interface;
    interface2.setData("Copied!");
    interface2.increment();

    std::cout << "Original - Data: " << interface.getData() << ", Counter: " << interface.getCounter() << std::endl;
    std::cout << "Copy - Data: " << interface2.getData() << ", Counter: " << interface2.getCounter() << std::endl;

    std::cout << "\nPimpl demonstration completed!" << std::endl;
    std::cout << "Benefits of Pimpl:" << std::endl;
    std::cout << "- Implementation details hidden from public interface" << std::endl;
    std::cout << "- Reduced compilation dependencies" << std::endl;
    std::cout << "- Binary compatibility (ABI stability)" << std::endl;
    std::cout << "- Implementation can change without affecting clients" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(PimplSample, "Pimpl", 4);