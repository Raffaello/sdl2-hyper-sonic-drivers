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
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}