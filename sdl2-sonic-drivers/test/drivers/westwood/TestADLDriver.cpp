#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drivers/westwood/ADLDriver.hpp>
#include <memory>
#include <audio/SDL2Mixer.hpp>
#include <files/ADLFile.hpp>
#include <hardware/opl/scummvm/mame/mame.hpp>

namespace drivers
{
    namespace westwood
    {
        TEST(ADLDriver, cstor)
        {
            std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
            std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("fixtures/DUNE19.ADL");
            std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);

            ADLDriver adlDrv(opl, adlFile);
        }

        TEST(ADLDriver, shared_ptr)
        {
            std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
            std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("fixtures/DUNE19.ADL");
            std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);

            std::shared_ptr<ADLDriver> adlDrv = std::make_shared<ADLDriver>(opl, adlFile);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
