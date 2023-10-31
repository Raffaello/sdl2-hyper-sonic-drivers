#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <std/IDeviceTypesFormatter.hpp>

namespace std
{
    TEST(IDeviceTypesFormatter, Music)
    {
        ASSERT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::Adlib).c_str(), "Adlib");
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
