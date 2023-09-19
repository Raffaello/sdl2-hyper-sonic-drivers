#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/midi/MidiAdlib.hpp>
#include <HyperSonicDrivers/drivers/MIDDriverMock.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/files/dmx/OP2File.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/devices/midi/EmulatorTestCase.hpp>

namespace HyperSonicDrivers::devices::midi
{
    TEST(MidiAdlib, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(auto a = std::make_shared<MidiAdlib>(mixer, op2File.getBank(), eChannelGroup::Plain));
    }

    class AdliblEmulator_ : public EmulatorTestCase<MidiAdlib> {};
    TEST_P(AdliblEmulator_, cstr_TYPE)
    {
        test_case();
    }

    INSTANTIATE_TEST_SUITE_P(
        MidiAdlib,
        AdliblEmulator_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::AUTO, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplEmulator::MAME, false),
            std::make_tuple<>(OplEmulator::WOODY, false),
            std::make_tuple<>(OplEmulator::NUKED, true) // TODO: this is OPL2 compatible so, it should work, at the moment forced only to be OPL3 type
        )
    );

    TEST(MidiAdlib, cstr_AUTO)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(MidiAdlib(mixer, op2File.getBank(), eChannelGroup::Plain));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
