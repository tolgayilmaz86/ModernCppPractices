#pragma once

#include "Testable.hpp"
#include <string>
#include <vector>
#include <memory>

class MoveSemanticsSample : public Testable {
public:
    void run() override;
    std::string name() const override { return "Move Semantics"; }

private:
    // Demonstration functions
    void demonstrate_move_constructors();
    void demonstrate_move_assignment();
    void demonstrate_rule_of_five();
    void demonstrate_std_move_usage();
    void demonstrate_move_with_containers();
    void demonstrate_perfect_forwarding();
    void demonstrate_rvo_vs_move();
    void demonstrate_move_semantics_best_practices();

    // Helper classes for demonstration
    class Resource {
    private:
        std::string name_;
        static int instance_count_;

    public:
        explicit Resource(const std::string& name);
        Resource(const Resource& other);
        Resource(Resource&& other) noexcept;
        Resource& operator=(const Resource& other);
        Resource& operator=(Resource&& other) noexcept;
        ~Resource();

        const std::string& name() const { return name_; }
        static int getInstanceCount() { return instance_count_; }
    };

    class MoveOnlyResource {
    private:
        std::unique_ptr<int> data_;

    public:
        MoveOnlyResource();
        MoveOnlyResource(const MoveOnlyResource&) = delete;
        MoveOnlyResource& operator=(const MoveOnlyResource&) = delete;
        MoveOnlyResource(MoveOnlyResource&& other) noexcept;
        MoveOnlyResource& operator=(MoveOnlyResource&& other) noexcept;
        ~MoveOnlyResource() = default;

        void setValue(int value);
        int getValue() const;
    };

    class ContainerWithResources {
    private:
        std::vector<Resource> resources_;

    public:
        void addResource(const std::string& name);
        void addResource(Resource&& resource);
        void moveFrom(ContainerWithResources&& other);
        const std::vector<Resource>& getResources() const;
        void clear();
    };
};