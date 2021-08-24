#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drivers/westwood/ADLDriver.hpp>
#include <memory>
#include "../test/mocks/MockMixer.hpp"
#include <files/ADLFile.hpp>
#include <hardware/opl/scummvm/mame/mame.hpp>

namespace drivers
{
    namespace westwood
    {
        using audio::mocks::MockMixer;
        TEST(ADLDriver, cstor)
        {
            // TODO: Review the Mixer as it is used in the OPL just to query the frequency rate.
            std::shared_ptr<MockMixer> mixer = std::make_shared<MockMixer>();
            EXPECT_EQ(mixer.use_count(), 1);

            std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("fixtures/DUNE19.ADL");
            EXPECT_EQ(adlFile.use_count(), 1);

            std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);
            EXPECT_EQ(opl.use_count(), 1);
            EXPECT_EQ(mixer.use_count(), 2);

            ADLDriver adlDrv(opl, adlFile);
            EXPECT_EQ(opl.use_count(), 2);
            EXPECT_EQ(adlFile.use_count(), 2);
        }

        TEST(ADLDriver, shared_ptr)
        {
            std::shared_ptr<MockMixer> mixer = std::make_shared<MockMixer>();
            EXPECT_EQ(mixer.use_count(), 1);

            std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("fixtures/DUNE19.ADL");
            EXPECT_EQ(adlFile.use_count(), 1);

            std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);
            EXPECT_EQ(opl.use_count(), 1);
            EXPECT_EQ(mixer.use_count(), 2);

            std::shared_ptr<ADLDriver> adlDrv = std::make_shared<ADLDriver>(opl, adlFile);
            EXPECT_EQ(adlDrv.use_count(), 1);
            EXPECT_EQ(opl.use_count(), 2);
            EXPECT_EQ(adlFile.use_count(), 2);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
