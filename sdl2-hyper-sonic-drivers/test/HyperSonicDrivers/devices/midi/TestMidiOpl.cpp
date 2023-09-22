#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/midi/MidiOpl.hpp>
#include <HyperSonicDrivers/drivers/MIDDriverMock.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/files/dmx/OP2File.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/devices/midi/MidiOplDeviceMock.hpp>
#include <HyperSonicDrivers/devices/makers.hpp>

namespace HyperSonicDrivers::devices::midi
{
    using audio::stubs::StubMixer;
    using audio::opl::banks::OP2Bank;
    using hardware::opl::OplType;
    using hardware::opl::OplEmulator;
    using hardware::opl::OPLFactory;
    using hardware::opl::OPL;
    using files::dmx::OP2File;
    using files::dmx::OP2File;
    using audio::mixer::eChannelGroup;

    const std::string GENMIDI_OP2 = std::string("../fixtures/GENMIDI.OP2");

    TEST(MidiOpl, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(MidiOplDeviceMock(OplType::OPL2, OplEmulator::AUTO, mixer, op2File.getBank()));
        EXPECT_THROW(MidiOplDeviceMock(OplType::OPL2, OplEmulator::AUTO, mixer, nullptr), std::invalid_argument);
    }

    class OplEmulator_ : public ::testing::TestWithParam<std::tuple<OplType, OplEmulator, bool>>
    {
    public:
        const OplType oplType = std::get<0>(GetParam());
        const OplEmulator oplEmu = std::get<1>(GetParam());
        const bool shouldThrow = std::get<2>(GetParam());
        const OP2File op2File = OP2File(GENMIDI_OP2);
        const std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
    };
    TEST_P(OplEmulator_, cstr_type_emu)
    {
        if (this->shouldThrow) {
            EXPECT_THROW(
                MidiOplDeviceMock(this->oplType, this->oplEmu, this->mixer, this->op2File.getBank()),
                std::runtime_error
            );
        }
        else {
            EXPECT_NO_THROW(MidiOplDeviceMock(this->oplType, this->oplEmu, this->mixer, this->op2File.getBank()));
        }
    }

    INSTANTIATE_TEST_SUITE_P(
        MidiOpl,
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
