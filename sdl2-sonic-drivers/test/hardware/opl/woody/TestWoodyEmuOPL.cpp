#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/woody/WoodyEmuOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            TEST(WoodyEmuOPL, cstorDefault)
            {
                int rate = 44100;
                WoodyEmuOPL wemu(rate, false);
                EXPECT_EQ(wemu.getChipType(), ChipType::OPL3);
                EXPECT_EQ(wemu.getSampleRate(), rate);
            }


        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
