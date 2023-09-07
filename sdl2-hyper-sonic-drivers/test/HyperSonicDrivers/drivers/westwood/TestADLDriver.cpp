#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/mame/MameOPL2.hpp>

namespace HyperSonicDrivers::drivers::westwood
{
    // TODO: Review the tests
    using audio::stubs::StubMixer;
    using hardware::opl::OplType;

    TEST(ADLDriver, cstor)
    {
        auto mixer = std::make_shared<StubMixer>();
        auto adlFile = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE19.ADL");
        auto opl = std::make_shared<hardware::opl::scummvm::mame::MameOPL2>(OplType::OPL2, mixer);
        EXPECT_EQ(opl.use_count(), 1);
        EXPECT_EQ(mixer.use_count(), 2);

        ADLDriver adlDrv(opl, adlFile);
        EXPECT_EQ(opl.use_count(), 2);
        EXPECT_EQ(adlFile.use_count(), 2);
    }

    TEST(ADLDriver, shared_ptr)
    {
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_EQ(mixer.use_count(), 1);

        auto adlFile = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE19.ADL");
        EXPECT_EQ(adlFile.use_count(), 1);

        auto opl = std::make_shared<hardware::opl::scummvm::mame::MameOPL2>(OplType::OPL2, mixer);
        EXPECT_EQ(opl.use_count(), 1);
        EXPECT_EQ(mixer.use_count(), 2);

        auto adlDrv = std::make_shared<ADLDriver>(opl, adlFile);
        EXPECT_EQ(adlDrv.use_count(), 1);
        EXPECT_EQ(opl.use_count(), 2);
        EXPECT_EQ(adlFile.use_count(), 2);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
