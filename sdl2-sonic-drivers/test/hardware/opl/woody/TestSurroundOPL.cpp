#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/woody/SurroundOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            TEST(SurroundOPL, cstorDefault)
            {
                int rate = 44100;
                SurroundOPL sopl(rate, true);
                EXPECT_EQ(sopl.getChipType(), ChipType::OPL2_DUAL);
                EXPECT_EQ(sopl.getSampleRate(), rate);
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
