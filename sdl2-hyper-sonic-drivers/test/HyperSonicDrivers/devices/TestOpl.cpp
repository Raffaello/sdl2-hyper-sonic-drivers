#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/drivers/MIDDriverMock.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/devices/OplDeviceMock.hpp>

namespace HyperSonicDrivers::devices::midi
{
    using audio::stubs::StubMixer;
    using hardware::opl::OplType;
    using hardware::opl::OplEmulator;
    using hardware::opl::OPL;
    using audio::mixer::eChannelGroup;

    TEST(MidiOpl, cstor_)
    {
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(OplDeviceMock(OplType::OPL2, OplEmulator::AUTO, mixer));
    }

    class OplEmulator_ : public ::testing::TestWithParam<std::tuple<OplType, OplEmulator, bool>>
    {
    public:
        const OplType oplType = std::get<0>(GetParam());
        const OplEmulator oplEmu = std::get<1>(GetParam());
        const bool shouldFail = std::get<2>(GetParam());
        const std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
    };
    TEST_P(OplEmulator_, cstr_type_emu)
    {
        OplDeviceMock opl(this->oplType, this->oplEmu, this->mixer);
        EXPECT_EQ(opl.init(), !this->shouldFail);
    }

    INSTANTIATE_TEST_SUITE_P(
        Opl,
        OplEmulator_,
        ::testing::Values(
            std::make_tuple<>(OplType::OPL2, OplEmulator::AUTO, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::AUTO, false),
            std::make_tuple<>(OplType::OPL3, OplEmulator::AUTO, false),
            std::make_tuple<>(OplType::OPL2, OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplType::OPL3, OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplType::OPL2, OplEmulator::MAME, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::MAME, true),
            std::make_tuple<>(OplType::OPL3, OplEmulator::MAME, true),
            std::make_tuple<>(OplType::OPL2, OplEmulator::WOODY, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::WOODY, false),
            std::make_tuple<>(OplType::OPL3, OplEmulator::WOODY, true),
            std::make_tuple<>(OplType::OPL2, OplEmulator::NUKED, true), // TODO: this is OPL2 compatible so, it should work, at the moment forced only to be OPL3 type
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::NUKED, true), // TODO: this is OPL2 compatible so, it should work, at the moment forced only to be OPL3 type
            std::make_tuple<>(OplType::OPL3, OplEmulator::NUKED, false)
        )
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
