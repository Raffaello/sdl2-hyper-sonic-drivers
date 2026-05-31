#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <std/IDeviceTypesFormatter.hpp>

namespace std
{
TEST(IDeviceTypesFormatter, Music)
{
    EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::Adlib).c_str(), "Adlib");
#if HAS_MT32_EMU
    EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::Mt32).c_str(), "Mt32");
#endif
    EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::SbPro).c_str(), "SbPro");
    EXPECT_STRCASEEQ(std::format("{}", HyperSonicDrivers::devices::eDeviceName::SbPro2).c_str(), "SbPro2");
}
}    // namespace std

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
