#include <gtest/gtest.h>
#include "01_RAII/RAIISample.hpp"
#include "02_SFINAE/SFINAESample.hpp"
#include "03_CRTP/CRTPSample.hpp"
#include "04_PIMPL/PimplSample.hpp"
#include "05_RuleOfFive/RuleOfFiveSample.hpp"

TEST(HelloTest, BasicAssertions) {
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);
}

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}