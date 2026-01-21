#include <gtest/gtest.h>
#include "01_RAII/RAIISample.hpp"
#include "02_SFINAE/SFINAESample.hpp"
#include "03_CRTP/CRTPSample.hpp"
#include "04_PIMPL/PimplSample.hpp"
#include "05_RuleOfFive/RuleOfFiveSample.hpp"
#include "06_TypeErasure/TypeErasureSample.hpp"
#include "07_VariantVisitor/VariantVisitorSample.hpp"
#include "08_SmartPointers/SmartPointersSample.hpp"
#include "09_ExceptionSafety/ExceptionSafetySample.hpp"
#include "10_MoveSemantics/MoveSemanticsSample.hpp"

TEST(Samples, RAII) {
    RAIISample sample;
    // This will run the RAII demonstration
    // In a real test, you might want to verify specific behavior
    sample.run();
}

TEST(Samples, SFINAE) {
    SFINAESample sample;
    // This will run the SFINAE demonstration
    sample.run();
}

TEST(Samples, CRTP) {
    CRTPSample sample;
    // This will run the CRTP demonstration
    sample.run();
}

TEST(Samples, PIMPL) {
    PimplSample sample;
    // This will run the Pimpl demonstration
    sample.run();
}

TEST(Samples, RuleOfFive) {
    RuleOfFiveSample sample;
    // This will run the Rule of Five demonstration
    sample.run();
}

TEST(Samples, TypeErasure) {
    TypeErasureSample sample;
    // This will run the Type Erasure demonstration
    sample.run();
}

TEST(Samples, VariantVisitor) {
    VariantVisitorSample sample;
    // This will run the Variant Visitor demonstration
    sample.run();
}

TEST(Samples, SmartPointers) {
    SmartPointersSample sample;
    // This will run the Smart Pointers demonstration
    sample.run();
}

TEST(Samples, ExceptionSafety) {
    ExceptionSafetySample sample;
    // This will run the Exception Safety demonstration
    sample.run();
}

TEST(Samples, MoveSemantics) {
    MoveSemanticsSample sample;
    // This will run the Move Semantics demonstration
    sample.run();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}