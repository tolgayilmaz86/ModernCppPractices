#include <gtest/gtest.h>
#include "../src/01_RAII/RAIISample.hpp"

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}