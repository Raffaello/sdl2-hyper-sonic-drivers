#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/drivers/PCMDriver.hpp>

namespace HyperSonicDrivers::drivers
{
    TEST(PCMDriver, cstor)
    {
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
