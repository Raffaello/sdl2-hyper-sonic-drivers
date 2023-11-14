#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <std/IDeviceTypesFormatter.hpp>

namespace std
{
    TEST(IDeviceTypesFormatter, Music)
    {
        EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::Adlib).c_str(), "Adlib");
        EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::Mt32).c_str(), "Mt32");
        EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::SbPro).c_str(), "SbPro");
        EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::SbPro2).c_str(), "SbPro2");
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
