#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/woodyopl/WoodyEmuOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woodyopl
        {
            TEST(WoodyEmuOPL, cstorDefault)
            {
                WoodyEmuOPL wemu(44100, false);
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
