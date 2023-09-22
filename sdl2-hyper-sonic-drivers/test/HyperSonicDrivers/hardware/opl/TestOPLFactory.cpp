#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/woody/WoodyOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/dosbox/DosBoxOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/mame/MameOPL2.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/nuked/NukedOPL3.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    using audio::stubs::StubMixer;
    using namespace hardware::opl::scummvm;

    const static std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();

    class ConfigTest : public ::testing::TestWithParam<std::tuple<OplEmulator, OplType>> {};
    TEST_P(ConfigTest, create)
    {
        OplEmulator emu = std::get<0>(GetParam());
        OplType type = std::get<1>(GetParam());

        auto opl = OPLFactory::create(emu, type, mixer);
        EXPECT_TRUE(opl->init());
        EXPECT_NE(opl, nullptr);
        EXPECT_EQ(opl.use_count(), 1);
        OPL* w = nullptr;
        switch (emu)
        {
        case OplEmulator::MAME:
            EXPECT_NO_THROW(w = dynamic_cast<mame::MameOPL2*>(opl.get()));
            break;
        case OplEmulator::DOS_BOX:
        case OplEmulator::AUTO:
            EXPECT_NO_THROW(w = dynamic_cast<dosbox::DosBoxOPL*>(opl.get()));
            break;
        case OplEmulator::NUKED:
            EXPECT_NO_THROW(w = dynamic_cast<nuked::NukedOPL*>(opl.get()));
            break;
        case OplEmulator::WOODY:
            EXPECT_NO_THROW(w = dynamic_cast<woody::WoodyOPL*>(opl.get()));
            break;
        default:
            FAIL();
        }

        EXPECT_NE(w, nullptr);
    }
    INSTANTIATE_TEST_SUITE_P(
        create,
        ConfigTest,
        ::testing::Values(
            std::make_tuple<OplEmulator, OplType>(OplEmulator::MAME, OplType::OPL2),
            //std::make_tuple<OplEmulator, OplType>(OplEmulator::MAME, OplType::DUAL_OPL2),
            //std::make_tuple<OplEmulator, OplType>(OplEmulator::MAME, OplType::OPL3),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::DOS_BOX, OplType::OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::DOS_BOX, OplType::DUAL_OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::DOS_BOX, OplType::OPL3),
            //std::make_tuple<OplEmulator, OplType>(OplEmulator::NUKED, OplType::OPL2),
            //std::make_tuple<OplEmulator, OplType>(OplEmulator::NUKED, OplType::DUAL_OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::NUKED, OplType::OPL3),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::WOODY, OplType::OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::WOODY, OplType::DUAL_OPL2),
            //std::make_tuple<OplEmulator, OplType>(OplEmulator::WOODY, OplType::OPL3),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::AUTO, OplType::OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::AUTO, OplType::DUAL_OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::AUTO, OplType::OPL3)
        )
    );

    class ConfigTestNull : public ConfigTest {};
    TEST_P(ConfigTestNull, create_nullptr)
    {
        OplEmulator emu = std::get<0>(GetParam());
        OplType type = std::get<1>(GetParam());

        auto opl = OPLFactory::create(emu, type, mixer);
        EXPECT_EQ(opl, nullptr);
        EXPECT_EQ(opl.get(), nullptr);
    }
    INSTANTIATE_TEST_SUITE_P(
        create_nullptr,
        ConfigTestNull,
        ::testing::Values(
            std::make_tuple<OplEmulator, OplType>(OplEmulator::MAME, OplType::DUAL_OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::MAME, OplType::OPL3),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::NUKED, OplType::OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::NUKED, OplType::DUAL_OPL2),
            std::make_tuple<OplEmulator, OplType>(OplEmulator::WOODY, OplType::OPL3)
        )
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
