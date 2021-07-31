#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/woody/OPLChip.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            TEST(OPLChip, cstorDefault)
            {
                int rate = 44100;
                OPLChip opl(rate);
                EXPECT_EQ(opl.getSampleRate(), rate);
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
