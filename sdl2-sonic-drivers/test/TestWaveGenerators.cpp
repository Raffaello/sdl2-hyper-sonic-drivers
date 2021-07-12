#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <softsynths/generators/generators.hpp>

TEST(WaveGenerators, Square) {
    EXPECT_EQ(softsynths::generators::generateSquare<int8_t>(0, 100), 127);
    EXPECT_EQ(softsynths::generators::generateSquare<int8_t>(25, 100), 127);
    EXPECT_EQ(softsynths::generators::generateSquare<int8_t>(50, 100), -128);
    EXPECT_EQ(softsynths::generators::generateSquare<int8_t>(75, 100), 128);
}

TEST(WaveGenerators, Pulse) {
    FAIL();
}

TEST(WaveGenerators, Saw) {
    FAIL();
}

TEST(WaveGenerators, Triangle) {
    FAIL();
}

TEST(WaveGenerators, Sine) {
    FAIL();
}

TEST(WaveGenerators, generateWave) {
    FAIL();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}