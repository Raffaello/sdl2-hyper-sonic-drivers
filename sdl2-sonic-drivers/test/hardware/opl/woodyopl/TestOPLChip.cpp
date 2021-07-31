#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/woodyopl/OPLChip.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woodyopl
        {
            TEST(OPLChip, cstorDefault)
            {
                OPLChip opl();
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
