#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/opl.hpp>
#include <hardware/opl/OPL.hpp>
#include <hardware/opl/Config.hpp>
#include "../test/audio/stubs/StubMixer.hpp"
#include <memory>

namespace utils
{
    // NOTE: Only DOS_BOX OPL Emulator is detected correctly as a normal OPL chip.
    // NOTE: Disabled due to CI Linux and Mac

    using hardware::opl::OPL;
    using hardware::opl::Config;
    using hardware::opl::OplEmulator;
    using hardware::opl::OplType;
    using audio::stubs::StubMixer;

    class OplType_ : public ::testing::TestWithParam<std::tuple<OplEmulator, OplType, bool, bool>>
    {
    public:
        std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
        OplEmulator opl_emu = std::get<0>(GetParam());
        OplType opl_type = std::get<1>(GetParam());
        bool isOpl2 = std::get<2>(GetParam());
        bool isOpl3 = std::get<3>(GetParam());
        
        std::shared_ptr<OPL> opl = Config::create(opl_emu, opl_type, mixer);
    };
    TEST_P(OplType_, DetectOPL)
    {
        ASSERT_TRUE(this->opl->init());
        this->opl->start(nullptr);
        EXPECT_EQ(detectOPL2(this->opl), isOpl2);
        EXPECT_EQ(detectOPL3(this->opl), isOpl3);
        //this->opl->stop();
    }
    INSTANTIATE_TEST_SUITE_P(
        DetectOpl,
        OplType_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::MAME, OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, OplType::OPL3, true, true),
            std::make_tuple<>(OplEmulator::AUTO, OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::AUTO, OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::AUTO, OplType::OPL3, true, true),
            std::make_tuple<>(OplEmulator::NUKED, OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::NUKED, OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::NUKED, OplType::OPL3, true, true)
        )
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
