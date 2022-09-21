#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/dosbox/Chip.hpp>
#include <utils/algorithms.hpp>

namespace hardware::opl::dosbox
{
    TEST(Chip, cstor)
    {
        Chip c;
        for (int i = 0; i < 2; i++)
        {
            EXPECT_EQ(c.timer[i].counter, 0);
            EXPECT_EQ(c.timer[i].delay, 0.0);
            EXPECT_EQ(c.timer[i].enabled, false);
            EXPECT_EQ(c.timer[i].masked, false);
            EXPECT_EQ(c.timer[i].overflow, false);
            EXPECT_EQ(c.timer[i].startTime, 0.0);
        }
    }

    TEST(Chip, write)
    {
        Chip c;

        EXPECT_TRUE(c.write(2, 1));
        EXPECT_TRUE(c.write(3, 1));
        EXPECT_TRUE(c.write(4, 0x80));
        EXPECT_TRUE(c.write(4, 0x1));
        EXPECT_TRUE(c.timer[0].enabled);
    }

    TEST(Chip, read)
    {
        Chip c;

        EXPECT_EQ(c.read(), 0);
    }

    TEST(Chip, overflow)
    {
        // used for standard detection of opl chip
        Chip c;

        EXPECT_TRUE(c.write(2, 0xFF)); // set timer 1 to 0xFF
        EXPECT_TRUE(c.write(4, 0x21)); // unmask and start timer 1
        utils::delayMillis(100);
        EXPECT_EQ(c.read(), 192);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
