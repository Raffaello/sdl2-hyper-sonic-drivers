#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <softsynths/generators/generators.hpp>
#include <numeric>

TEST(WaveGenerators, Square)
{
    int f = 100;
    EXPECT_EQ(softsynths::generators::generateSquare<int8_t>(0, f), std::numeric_limits<int8_t>::max());
    EXPECT_EQ(softsynths::generators::generateSquare<int8_t>(f/2, f), std::numeric_limits<int8_t>::min());

    EXPECT_EQ(softsynths::generators::generateSquare<int16_t>(0, f), std::numeric_limits<int16_t>::max());
    EXPECT_EQ(softsynths::generators::generateSquare<int16_t>(f/2, f), std::numeric_limits<int16_t>::min());

    EXPECT_EQ(softsynths::generators::generateSquare<uint8_t>(0, f), std::numeric_limits<uint8_t>::max());
    EXPECT_EQ(softsynths::generators::generateSquare<uint8_t>(f/2, f), std::numeric_limits<uint8_t>::min());

    EXPECT_EQ(softsynths::generators::generateSquare<uint16_t>(0, f), std::numeric_limits<uint16_t>::max());
    EXPECT_EQ(softsynths::generators::generateSquare<uint16_t>(f/2, f), std::numeric_limits<uint16_t>::min());
}

TEST(DISABLED_WaveGenerators, Pulse)
{
    FAIL();
}

TEST(WaveGenerators, Saw)
{
    // 0% -> min, 50% -> ~0, 99% -> ~max
    int f = 100;
    
    EXPECT_EQ(softsynths::generators::generateSaw<int8_t>(0, f), std::numeric_limits<int8_t>::min());
    EXPECT_EQ(softsynths::generators::generateSaw<int8_t>(f * 1 / 2, f), -1);
    EXPECT_EQ(softsynths::generators::generateSaw<int8_t>(f, f), std::numeric_limits<int8_t>::max());
}

TEST(WaveGenerators, Triangle)
{
    // 0% -> min, 25% -> -0.5, 50% -> max, 75% -> 0.5
    int f = 100;

    EXPECT_EQ(
        softsynths::generators::generateTriangle<int8_t>(0, f),
        std::numeric_limits<int8_t>::min()
    );
    EXPECT_EQ(
        softsynths::generators::generateTriangle<int8_t>(0, f),
        softsynths::generators::generateTriangle<int8_t>(0, 2 * f)
    );
    EXPECT_EQ(softsynths::generators::generateTriangle<int8_t>(f * 1 / 4, f), -1);

    EXPECT_EQ(softsynths::generators::generateTriangle<int8_t>(f * 3 / 4, f), 0);

    /*EXPECT_EQ(
        softsynths::generators::generateTriangle<int8_t>(f * 1 / 4, f),
        softsynths::generators::generateTriangle<int8_t>(f * 3 / 4, f)
    );*/

    EXPECT_EQ(softsynths::generators::generateTriangle<int8_t>(f * 2 / 4, f), std::numeric_limits<int8_t>::max());
}

TEST(WaveGenerators, Sine)
{
    int f = 100;
    // 0% -> 0, 25% -> max, 50% -> 0, 75% -> min
    EXPECT_EQ(softsynths::generators::generateSine<int8_t>(0, f), 0);
    EXPECT_EQ(softsynths::generators::generateSine<int8_t>(f * 1 / 4, f), std::numeric_limits<int8_t>::max());
    EXPECT_EQ(softsynths::generators::generateSine<int8_t>(f * 2 / 4, f), 0);
    //EXPECT_EQ(softsynths::generators::generateSine<int8_t>(f * 3 / 4, f), std::numeric_limits<int8_t>::min());
    EXPECT_EQ(softsynths::generators::generateSine<int8_t>(f * 3 / 4, f), std::numeric_limits<int8_t>::min() + 1);

}

TEST(DISABLED_WaveGenerators, generateWave)
{
    //
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
