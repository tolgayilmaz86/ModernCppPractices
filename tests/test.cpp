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
#include "11_TagDispatching/TagDispatchingSample.hpp"
#include "12_DeepShallowCopy/DeepShallowCopySample.hpp"
#include "13_CopyAndSwap/CopyAndSwapSample.hpp"
#include "14_CastingTypes/CastingTypesSample.hpp"
#include "15_ThreadSafety/ThreadSafetySample.hpp"
#include "16_Concepts/ConceptsSample.hpp"
#include "17_Coroutines/CoroutinesSample.hpp"
#include "18_SRP/SRPSample.hpp"
#include "19_OCP/OCPSample.hpp"
#include "20_LSP/LSPSample.hpp"
#include "21_ISP/ISPSample.hpp"
#include "22_DIP/DIPSample.hpp"
#include "23_UMLRelationships/UMLRelationshipsSample.hpp"

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

TEST(Samples, TagDispatching) {
    TagDispatchingSample sample;
    // This will run the Tag Dispatching demonstration
    sample.run();
}

TEST(Samples, DeepShallowCopy) {
    DeepShallowCopySample sample;
    // This will run the Deep vs Shallow Copy demonstration
    sample.run();
}

TEST(Samples, CopyAndSwapIdiom) {
    CopyAndSwapSample sample;
    // This will run the Copy and Swap Idiom demonstration
    sample.run();
}

TEST(Samples, Cast) {
    CastingTypesSample sample;
    // This will run the Cast Types demonstration
    sample.run();
}

TEST(Samples, ThreadSafety) {
    ThreadSafetySample sample;
    // This will run the Thread Safety demonstration
    sample.run();
}

TEST(Samples, Concepts) {
    ConceptsSample sample;
    // This will run the C++20 Concepts demonstration
    sample.run();
}

TEST(Samples, Coroutines) {
    CoroutinesSample sample;
    // This will run the C++20 Coroutines demonstration
    sample.run();
}

TEST(SOLID, SingleResponsibilityPrinciple) {
    SRPSample sample;
    // This will run the Single Responsibility Principle demonstration
    sample.run();
}

TEST(SOLID, OpenClosedPrinciple) {
    OCPSample sample;
    // This will run the Open/Closed Principle demonstration
    sample.run();
}

TEST(SOLID, LiskovSubstitutionPrinciple) {
    LSPSample sample;
    // This will run the Liskov Substitution Principle demonstration
    sample.run();
}

TEST(SOLID, InterfaceSegregationPrinciple) {
    ISPSample sample;
    // This will run the Interface Segregation Principle demonstration
    sample.run();
}

TEST(SOLID, DependencyInversionPrinciple) {
    DIPSample sample;
    // This will run the Dependency Inversion Principle demonstration
    sample.run();
}

TEST(OOP, UMLRelationships) {
    UMLRelationshipsSample sample;
    // This will run the UML Relationships demonstration
    sample.run();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}