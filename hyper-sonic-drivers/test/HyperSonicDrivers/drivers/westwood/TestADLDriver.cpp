#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <HyperSonicDrivers/devices/Adlib.hpp>

namespace HyperSonicDrivers::drivers::westwood
{
    // TODO: Review the tests
    using audio::IMixerMock;
    using hardware::opl::OplType;

    TEST(ADLDriver, cstor)
    {
        auto mixer = std::make_shared<IMixerMock>();
        auto adlFile = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE19.ADL");
        auto adlib = devices::make_device<devices::Adlib, devices::Opl>(mixer);
        ADLDriver adlDrv(adlib, audio::mixer::eChannelGroup::Plain);
        adlDrv.setADLFile(adlFile);
        EXPECT_EQ(adlFile.use_count(), 2);
    }

    TEST(ADLDriver, shared_ptr)
    {
        auto mixer = std::make_shared<IMixerMock>();
        EXPECT_EQ(mixer.use_count(), 1);

        auto adlFile = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE19.ADL");
        EXPECT_EQ(adlFile.use_count(), 1);

        auto adlib = devices::make_device<devices::Adlib, devices::Opl>(mixer);

        auto adlDrv = std::make_shared<ADLDriver>(adlib, audio::mixer::eChannelGroup::Plain);
        adlDrv->setADLFile(adlFile);
        EXPECT_EQ(adlDrv.use_count(), 1);
        EXPECT_EQ(adlFile.use_count(), 2);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
