#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/Timer.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    TEST(Timer, cstor)
    {
        Timer t;
        EXPECT_EQ(t.counter, 0);
        EXPECT_EQ(t.delay, 0);
        EXPECT_EQ(t.enabled, false);
        EXPECT_EQ(t.masked, false);
        EXPECT_EQ(t.overflow, false);
        EXPECT_EQ(t.startTime, 0);
    }

    TEST(Timer, start)
    {
        Timer t;
        t.start(0, 1);
        EXPECT_EQ(t.enabled, true);
        EXPECT_EQ(t.counter, 0);
        EXPECT_EQ(t.delay, 0.001 * 256);
        EXPECT_EQ(t.startTime, 0 + 0.001*256);
    }

    TEST(Timer, stop)
    {
        Timer t;
        t.enabled = true;
        t.stop();
        EXPECT_EQ(t.enabled, false);
    }

    TEST(Timer, update)
    {
        Timer t;

        t.start(0, 1);
        t.update(1.0);
        EXPECT_EQ(t.masked, false);
        EXPECT_EQ(t.overflow, true);
    }
    TEST(Timer, update_masked)
    {
        Timer t;

        t.masked = true;
        t.start(0, 1);
        t.update(1.0);
        EXPECT_EQ(t.masked, true);
        EXPECT_EQ(t.overflow, false);
    }

    TEST(Timer, reset)
    {
        Timer t;

        t.start(0, 1);
        t.update(1.0);
        EXPECT_EQ(t.masked, false);
        EXPECT_EQ(t.overflow, true);

        t.reset(0.0);
        EXPECT_EQ(t.overflow, false);
        EXPECT_EQ(t.startTime, 0.256);

    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
