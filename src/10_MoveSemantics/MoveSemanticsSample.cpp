#include "MoveSemanticsSample.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>

// ============================================================================
// Resource Class Implementation
// ============================================================================

int MoveSemanticsSample::Resource::instance_count_ = 0;

MoveSemanticsSample::Resource::Resource(const std::string& name) : name_(name) {
    instance_count_++;
    std::cout << "Resource '" << name_ << "' constructed. Total instances: " << instance_count_ << std::endl;
}

MoveSemanticsSample::Resource::Resource(const Resource& other) : name_(other.name_ + "_copy") {
    instance_count_++;
    std::cout << "Resource '" << name_ << "' copy-constructed from '" << other.name_ << "'. Total instances: " << instance_count_ << std::endl;
}

MoveSemanticsSample::Resource::Resource(Resource&& other) noexcept : name_(std::move(other.name_)) {
    instance_count_++;
    other.name_.clear();
    std::cout << "Resource '" << name_ << "' move-constructed. Total instances: " << instance_count_ << std::endl;
}

MoveSemanticsSample::Resource& MoveSemanticsSample::Resource::operator=(const Resource& other) {
    if (this != &other) {
        name_ = other.name_ + "_assigned";
        std::cout << "Resource '" << name_ << "' copy-assigned" << std::endl;
    }
    return *this;
}

MoveSemanticsSample::Resource& MoveSemanticsSample::Resource::operator=(Resource&& other) noexcept {
    if (this != &other) {
        name_ = std::move(other.name_);
        other.name_.clear();
        std::cout << "Resource '" << name_ << "' move-assigned" << std::endl;
    }
    return *this;
}

MoveSemanticsSample::Resource::~Resource() {
    instance_count_--;
    std::cout << "Resource '" << name_ << "' destroyed. Total instances: " << instance_count_ << std::endl;
}

// ============================================================================
// MoveOnlyResource Class Implementation
// ============================================================================

MoveSemanticsSample::MoveOnlyResource::MoveOnlyResource() : data_(std::make_unique<int>(0)) {
    std::cout << "MoveOnlyResource constructed" << std::endl;
}

MoveSemanticsSample::MoveOnlyResource::MoveOnlyResource(MoveOnlyResource&& other) noexcept : data_(std::move(other.data_)) {
    std::cout << "MoveOnlyResource move-constructed" << std::endl;
}

MoveSemanticsSample::MoveOnlyResource& MoveSemanticsSample::MoveOnlyResource::operator=(MoveOnlyResource&& other) noexcept {
    if (this != &other) {
        data_ = std::move(other.data_);
        std::cout << "MoveOnlyResource move-assigned" << std::endl;
    }
    return *this;
}

void MoveSemanticsSample::MoveOnlyResource::setValue(int value) {
    if (data_) {
        *data_ = value;
    }
}

int MoveSemanticsSample::MoveOnlyResource::getValue() const {
    return data_ ? *data_ : 0;
}

// ============================================================================
// ContainerWithResources Class Implementation
// ============================================================================

void MoveSemanticsSample::ContainerWithResources::addResource(const std::string& name) {
    resources_.emplace_back(name);
}

void MoveSemanticsSample::ContainerWithResources::addResource(Resource&& resource) {
    resources_.push_back(std::move(resource));
}

void MoveSemanticsSample::ContainerWithResources::moveFrom(ContainerWithResources&& other) {
    resources_ = std::move(other.resources_);
    other.resources_.clear();
}

const std::vector<MoveSemanticsSample::Resource>& MoveSemanticsSample::ContainerWithResources::getResources() const {
    return resources_;
}

void MoveSemanticsSample::ContainerWithResources::clear() {
    resources_.clear();
}

// ============================================================================
// Demonstration Functions
// ============================================================================

void MoveSemanticsSample::demonstrate_move_constructors() {
    std::cout << "\n=== Move Constructors ===" << std::endl;

    std::cout << "--- Creating original resource ---" << std::endl;
    Resource original("Original");

    std::cout << "\n--- Copy construction (expensive) ---" << std::endl;
    Resource copy = original;

    std::cout << "\n--- Move construction (efficient) ---" << std::endl;
    Resource moved = std::move(original);

    std::cout << "\nOriginal after move: '" << original.name() << "'" << std::endl;
    std::cout << "Copy: '" << copy.name() << "'" << std::endl;
    std::cout << "Moved: '" << moved.name() << "'" << std::endl;
}

void MoveSemanticsSample::demonstrate_move_assignment() {
    std::cout << "\n=== Move Assignment ===" << std::endl;

    std::cout << "--- Creating resources ---" << std::endl;
    Resource target("Target");
    Resource source("Source");

    std::cout << "\n--- Copy assignment (expensive) ---" << std::endl;
    Resource copy_target("CopyTarget");
    copy_target = source;

    std::cout << "\n--- Move assignment (efficient) ---" << std::endl;
    target = std::move(source);

    std::cout << "\nSource after move: '" << source.name() << "'" << std::endl;
    std::cout << "Target after move: '" << target.name() << "'" << std::endl;
}

void MoveSemanticsSample::demonstrate_rule_of_five() {
    std::cout << "\n=== Rule of Five ===" << std::endl;

    std::cout << "--- Resource class implements all five special member functions ---" << std::endl;
    std::cout << "1. Destructor" << std::endl;
    std::cout << "2. Copy constructor" << std::endl;
    std::cout << "3. Copy assignment operator" << std::endl;
    std::cout << "4. Move constructor" << std::endl;
    std::cout << "5. Move assignment operator" << std::endl;

    std::cout << "\n--- Demonstrating Rule of Five ---" << std::endl;
    {
        Resource res1("RuleOfFive_1");
        Resource res2 = res1;  // Copy constructor
        Resource res3 = std::move(res1);  // Move constructor
        res2 = std::move(res3);  // Move assignment
    }  // All destructors called here
}

void MoveSemanticsSample::demonstrate_std_move_usage() {
    std::cout << "\n=== std::move Usage ===" << std::endl;

    std::cout << "--- When to use std::move ---" << std::endl;
    std::cout << "1. Moving from lvalues that won't be used again" << std::endl;
    std::cout << "2. Implementing move constructors/assignment" << std::endl;
    std::cout << "3. Passing ownership to containers or functions" << std::endl;

    std::cout << "\n--- Example: Moving into container ---" << std::endl;
    std::vector<Resource> resources;
    Resource temp("TempResource");

    // Move temp into vector - temp won't be used again
    resources.push_back(std::move(temp));
    std::cout << "Temp after move: '" << temp.name() << "'" << std::endl;

    std::cout << "\n--- Example: Function returning by value ---" << std::endl;
    auto create_resource = []() -> Resource {
        Resource local("LocalResource");
        return local;  // Automatic move (RVO or move)
    };

    Resource returned = create_resource();
    std::cout << "Returned resource: '" << returned.name() << "'" << std::endl;
}

void MoveSemanticsSample::demonstrate_move_with_containers() {
    std::cout << "\n=== Move Semantics with Containers ===" << std::endl;

    std::cout << "--- Creating container with resources ---" << std::endl;
    ContainerWithResources container1;
    container1.addResource("Res1");
    container1.addResource("Res2");
    container1.addResource("Res3");

    std::cout << "\n--- Moving entire container ---" << std::endl;
    ContainerWithResources container2;
    container2.moveFrom(std::move(container1));

    std::cout << "Container1 size after move: " << container1.getResources().size() << std::endl;
    std::cout << "Container2 size after move: " << container2.getResources().size() << std::endl;

    std::cout << "\n--- Moving individual elements ---" << std::endl;
    ContainerWithResources container3;
    Resource individual("Individual");
    container3.addResource(std::move(individual));
    std::cout << "Individual after move: '" << individual.name() << "'" << std::endl;
}

void MoveSemanticsSample::demonstrate_perfect_forwarding() {
    std::cout << "\n=== Perfect Forwarding ===" << std::endl;

    std::cout << "--- Perfect forwarding preserves value category ---" << std::endl;

    auto wrapper_function = [](auto&& arg) {
        std::cout << "Argument type: ";
        if constexpr (std::is_lvalue_reference_v<decltype(arg)>) {
            std::cout << "lvalue reference" << std::endl;
        } else {
            std::cout << "rvalue reference" << std::endl;
        }
        return std::forward<decltype(arg)>(arg);
    };

    Resource lvalue("Lvalue");
    std::cout << "Passing lvalue: ";
    Resource result1 = wrapper_function(lvalue);

    std::cout << "Passing rvalue: ";
    Resource result2 = wrapper_function(Resource("Rvalue"));

    std::cout << "Passing moved lvalue: ";
    Resource result3 = wrapper_function(std::move(lvalue));
}

void MoveSemanticsSample::demonstrate_rvo_vs_move() {
    std::cout << "\n=== RVO vs Move Semantics ===" << std::endl;

    std::cout << "--- Return Value Optimization (RVO) ---" << std::endl;
    std::cout << "Compiler can elide copies even without move semantics" << std::endl;

    auto create_with_rvo = []() -> Resource {
        Resource local("RVO_Resource");
        std::cout << "Inside function, about to return" << std::endl;
        return local;  // RVO: no copy/move happens
    };

    std::cout << "Calling function that benefits from RVO:" << std::endl;
    Resource rvo_result = create_with_rvo();

    std::cout << "\n--- When RVO doesn't apply ---" << std::endl;
    auto create_without_rvo = []() -> Resource {
        Resource res1("Res1");
        Resource res2("Res2");
        if (rand() % 2) {
            return res1;  // Move happens here
        } else {
            return res2;  // Move happens here
        }
    };

    std::cout << "Calling function where RVO doesn't apply:" << std::endl;
    Resource no_rvo_result = create_without_rvo();
}

void MoveSemanticsSample::demonstrate_move_semantics_best_practices() {
    std::cout << "\n=== Move Semantics Best Practices ===" << std::endl;

    std::cout << "--- 1. Move-only types ---" << std::endl;
    {
        MoveOnlyResource res1;
        res1.setValue(42);
        MoveOnlyResource res2 = std::move(res1);  // Move construction
        std::cout << "Moved value: " << res2.getValue() << std::endl;
    }

    std::cout << "\n--- 2. Don't move from const objects ---" << std::endl;
    const Resource const_res("ConstResource");
    // const_res = std::move(some_other);  // Error: can't move to const

    std::cout << "\n--- 3. Be careful with self-assignment in move assignment ---" << std::endl;
    Resource self_test("SelfTest");
    self_test = std::move(self_test);  // Safe due to self-check

    std::cout << "\n--- 4. Use noexcept for move operations ---" << std::endl;
    std::cout << "Move operations should be noexcept to enable optimizations" << std::endl;

    std::cout << "\n--- 5. Don't assume moved-from objects are empty ---" << std::endl;
    std::cout << "Moved-from objects should be in valid but unspecified state" << std::endl;
}

#include "SampleRegistry.hpp"

void MoveSemanticsSample::run() {
    std::cout << "Running Move Semantics Sample..." << std::endl;

    demonstrate_move_constructors();
    demonstrate_move_assignment();
    demonstrate_rule_of_five();
    demonstrate_std_move_usage();
    demonstrate_move_with_containers();
    demonstrate_perfect_forwarding();
    demonstrate_rvo_vs_move();
    demonstrate_move_semantics_best_practices();

    std::cout << "\n=== Move Semantics Summary ===" << std::endl;
    std::cout << "Move semantics enable efficient resource transfer in C++11+" << std::endl;
    std::cout << "- Use std::move() to convert lvalues to rvalues" << std::endl;
    std::cout << "- Implement Rule of Five for classes managing resources" << std::endl;
    std::cout << "- Move operations should be noexcept when possible" << std::endl;
    std::cout << "- RVO can be better than move in some cases" << std::endl;
    std::cout << "- Perfect forwarding preserves value categories" << std::endl;
    std::cout << "- Move semantics work seamlessly with containers" << std::endl;

    std::cout << "\nMove semantics demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(MoveSemanticsSample, "Move Semantics", 10);