#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/hardware/opl/Chip.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    class ChipMock : public Chip
    {
    public:

        Timer getTimer(const int i)
        {
            return this->timer[i];
        }
    };
    TEST(Chip, cstor)
    {
        ChipMock c;
        for (int i = 0; i < 2; i++)
        {
            EXPECT_EQ(c.getTimer(i).counter, 0);
            EXPECT_EQ(c.getTimer(i).delay, 0.0);
            EXPECT_EQ(c.getTimer(i).enabled, false);
            EXPECT_EQ(c.getTimer(i).masked, false);
            EXPECT_EQ(c.getTimer(i).overflow, false);
            EXPECT_EQ(c.getTimer(i).startTime, 0.0);
        }
    }

    TEST(Chip, write)
    {
        ChipMock c;

        EXPECT_TRUE(c.write(2, 1));
        EXPECT_TRUE(c.write(3, 1));
        EXPECT_TRUE(c.write(4, 0x80));
        EXPECT_TRUE(c.write(4, 0x1));
        EXPECT_TRUE(c.getTimer(0).enabled);
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
