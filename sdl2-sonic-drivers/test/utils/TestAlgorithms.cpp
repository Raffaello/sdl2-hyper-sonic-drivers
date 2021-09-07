#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/algorithms.hpp>

namespace utils
{
    class GCDTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {};
    TEST_P(GCDTest, gcd)
    {
        int a = std::get<0>(GetParam());
        int b = std::get<1>(GetParam());
        int exp = std::get<2>(GetParam());
        // GTEST_LOG_(INFO) << "a=" << a << " --- b=" << b << " --- exp=" << exp;
        EXPECT_EQ(gcd<int>(a, b), exp);
        EXPECT_EQ(gcd<int>(b, a), exp);
    }
    INSTANTIATE_TEST_SUITE_P(
        Algorithms,
        GCDTest,
        ::testing::Values(
            std::make_tuple<int, int, int>(10, 5, 5),
            std::make_tuple<int, int, int>(1, 1, 1),
            std::make_tuple<int, int, int>(2, 1, 1),
            std::make_tuple<int, int, int>(13, 30, 1),
            std::make_tuple<int, int, int>(12, 30, 6),
            std::make_tuple<int, int, int>(100, 100, 100),
            std::make_tuple<int, int, int>(100, 10, 10),
            std::make_tuple<int, int, int>(100, 80, 20)
        )
    );

    TEST(Algorithms, CLIP)
    {
        EXPECT_EQ(CLIP<int>(10, 0, 12), 10);
        EXPECT_EQ(CLIP<int>(10, 0, 9), 9);
        EXPECT_EQ(CLIP<int>(10, 11, 12), 11);
    }

    TEST(Algorithms, ARRAYCLEAR)
    {
        const int size = 5;
        int buf[size];
        ARRAYCLEAR<int, size>(buf, 3);
        EXPECT_THAT(buf, ::testing::ElementsAre(3, 3, 3, 3, 3));
    }

    class VLQTest : public ::testing::TestWithParam<std::tuple<std::vector<uint8_t>, uint32_t, int>> {};
    TEST_P(VLQTest, variable_length_quantities)
    {
        auto values = std::get<0>(GetParam());
        uint32_t exp_value = std::get<1>(GetParam());
        int exp_reads = std::get<2>(GetParam());
        const uint8_t* buf = values.data();
        uint32_t value = 0;

        int reads = utils::decode_VLQ(buf, value);
        EXPECT_EQ(value, exp_value);
        EXPECT_EQ(reads, exp_reads);
    }
    INSTANTIATE_TEST_SUITE_P(
        MIDFile,
        VLQTest,
        ::testing::Values(
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0 }, 0, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x40 }, 0x40, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x7F }, 0x7F, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0 }, 0x80, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0 }, 0x2000, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0x7F }, 0x3FFF, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0x80, 0 }, 0x4000, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0x80, 0 }, 0x100000, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0xFF, 0x7F }, 0x1FFFFF, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0x80, 0x80, 0 }, 0x200000, 4),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0x80, 0x80, 0 }, 0x8000000, 4),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0xFF, 0xFF, 0x7F }, 0x0FFFFFFF, 4)
        )
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
