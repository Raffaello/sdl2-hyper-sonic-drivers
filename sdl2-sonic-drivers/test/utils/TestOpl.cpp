#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/opl.hpp>
#include <hardware/opl/OPL.hpp>
#include <hardware/opl/scummvm/Config.hpp>
#include "../test/audio/stubs/StubMixer.hpp"
#include <memory>

namespace utils
{
    // NOTE: Only DOS_BOX OPL Emulator is detected correctly as a normal OPL chip.
    // NOTE: Disabled due to CI Linux and Mac

    using hardware::opl::OPL;
    using hardware::opl::scummvm::Config;
    using hardware::opl::scummvm::OplEmulator;
    using audio::stubs::StubMixer;

    class OplType : public ::testing::TestWithParam<std::tuple<OplEmulator, Config::OplType, bool, bool>>
    {
    public:
        std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
        OplEmulator opl_emu = std::get<0>(GetParam());
        Config::OplType opl_type = std::get<1>(GetParam());
        bool isOpl2 = std::get<2>(GetParam());
        bool isOpl3 = std::get<3>(GetParam());
        
        std::shared_ptr<OPL> opl = Config::create(opl_emu, opl_type, mixer);
    };
    TEST_P(OplType, DetectOPL)
    {
        EXPECT_EQ(detectOPL2(this->opl), isOpl2);
        EXPECT_EQ(detectOPL3(this->opl), isOpl3);
    }
    INSTANTIATE_TEST_SUITE_P(
        DISABLED_DetectOpl,
        OplType,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::DOS_BOX, Config::OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, Config::OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, Config::OplType::OPL3, true, true),
            std::make_tuple<>(OplEmulator::AUTO, Config::OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::AUTO, Config::OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::AUTO, Config::OplType::OPL3, true, true)
        )
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
