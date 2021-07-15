#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <softsynths/generators/generators.hpp>

using softsynths::generators::eWaveForm;

template<typename T> class WaveGeneratorTest : public ::testing::Test
{
public:
    const int f = 100;
    const T max_ = std::numeric_limits<T>::max();
    const T min_ = std::numeric_limits<T>::min();
    const int32_t mid_ = softsynths::generators::mid<T>;
    const std::function<T(uint32_t, uint32_t)> wave = nullptr;
    const std::function<T(eWaveForm, uint32_t, uint32_t)> genWave = softsynths::generators::generateWave<T>;
};

template<class T> class SquareWaveGeneratorTest : public WaveGeneratorTest<T>
{
public:
    const std::function<T(uint32_t, uint32_t)>wave = softsynths::generators::generateSquare<T>;
};
//TODO: int32_t, float 32bits
using Types = ::testing::Types<int8_t, int16_t, uint8_t, uint16_t>;
TYPED_TEST_SUITE(SquareWaveGeneratorTest, Types);
TYPED_TEST(SquareWaveGeneratorTest, percent_0)
{
    EXPECT_EQ(this->wave(0, this->f), this->max_);
    EXPECT_EQ(this->genWave(eWaveForm::SQUARE, 0, this->f), this->max_);

}
TYPED_TEST(SquareWaveGeneratorTest, percent_50)
{
    EXPECT_EQ(this->wave(this->f * 1 / 2, this->f), this->min_);
    EXPECT_EQ(this->genWave(eWaveForm::SQUARE, this->f * 1 / 2, this->f), this->min_);
}
TYPED_TEST(SquareWaveGeneratorTest, percent_100)
{
    EXPECT_EQ(this->wave(this->f, this->f), this->min_);
    EXPECT_EQ(this->genWave(eWaveForm::SQUARE, this->f, this->f), this->min_);
}

template<class T> class SawWaveGeneratorTest : public WaveGeneratorTest<T>
{
public:
    const std::function<T(uint32_t, uint32_t)>wave = softsynths::generators::generateSaw<T>;
};
// 0% -> min, 50% -> ~0, 99% -> ~max
TYPED_TEST_SUITE(SawWaveGeneratorTest, Types);
TYPED_TEST(SawWaveGeneratorTest, percent_0)
{
    EXPECT_EQ(this->wave(0, f), this->min_);
    EXPECT_EQ(this->genWave(eWaveForm::SAW, 0, f), this->min_);
}
TYPED_TEST(SawWaveGeneratorTest, percent_50)
{
    EXPECT_NEAR(this->wave(f * 1 / 2, f), this->mid_, 1);
    EXPECT_NEAR(this->genWave(eWaveForm::SAW, f * 1 / 2, f), this->mid_, 1);
}
TYPED_TEST(SawWaveGeneratorTest, percent_100)
{
    EXPECT_EQ(this->wave(f, f), this->max_);
    EXPECT_EQ(this->genWave(eWaveForm::SAW, f, f), this->max_);
}

template<class T> class TriangleWaveGeneratorTest : public WaveGeneratorTest<T>
{
public:
    const std::function<T(uint32_t, uint32_t)>wave = softsynths::generators::generateTriangle<T>;
};
// 0% -> min, 25% -> -0.5, 50% -> max, 75% -> 0.5
TYPED_TEST_SUITE(TriangleWaveGeneratorTest, Types);
TYPED_TEST(TriangleWaveGeneratorTest, percent_0)
{
    EXPECT_EQ(this->wave(0, f), this->min_);
    EXPECT_EQ(this->wave(0, f), this->wave(f, f));
    EXPECT_EQ(this->genWave(eWaveForm::TRIANGLE, 0, f), this->min_);
}
TYPED_TEST(TriangleWaveGeneratorTest, percent_25)
{
    EXPECT_NEAR(this->wave(f * 1 / 4, f), this->mid_, 1);
    EXPECT_NEAR(this->genWave(eWaveForm::TRIANGLE, f * 1 / 4, f), this->mid_, 1);
}
TYPED_TEST(TriangleWaveGeneratorTest, percent_50)
{
    EXPECT_EQ(this->wave(f * 2 / 4, f), this->max_);
    EXPECT_EQ(this->genWave(eWaveForm::TRIANGLE, f * 2 / 4, f), this->max_);
}
TYPED_TEST(TriangleWaveGeneratorTest, percent_75)
{
    EXPECT_NEAR(this->wave(f * 3 / 4, f), this->mid_, 1);
    EXPECT_NEAR(this->genWave(eWaveForm::TRIANGLE, f * 3 / 4, f), this->mid_, 1);
}

template<class T> class SineWaveGeneratorTest : public WaveGeneratorTest<T>
{
public:
    const std::function<T(uint32_t, uint32_t)>wave = softsynths::generators::generateSine<T>;
};
// 0% -> 0, 25% -> max, 50% -> 0, 75% -> min
TYPED_TEST_SUITE(SineWaveGeneratorTest, Types);
TYPED_TEST(SineWaveGeneratorTest, percent_0)
{
    EXPECT_EQ(this->wave(0, f), this->mid_);
    EXPECT_EQ(this->wave(f, f), this->mid_);
    EXPECT_EQ(this->genWave(eWaveForm::SINE, 0, f), this->mid_);
}
TYPED_TEST(SineWaveGeneratorTest, percent_25)
{
    EXPECT_NEAR(this->wave(f * 1 / 4, f), this->max_, 1);
    EXPECT_NEAR(this->genWave(eWaveForm::SINE, f * 1 / 4, f), this->max_, 1);
}
TYPED_TEST(SineWaveGeneratorTest, percent_50)
{
    EXPECT_EQ(this->wave(f * 2 / 4, f), this->mid_);
    EXPECT_EQ(this->genWave(eWaveForm::SINE, f * 2 / 4, f), this->mid_);
}
TYPED_TEST(SineWaveGeneratorTest, percent_75)
{
    EXPECT_NEAR(this->wave(f * 3 / 4, f), this->min_, 1);
    EXPECT_NEAR(this->genWave(eWaveForm::SINE, f * 3 / 4, f), this->min_, 1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
