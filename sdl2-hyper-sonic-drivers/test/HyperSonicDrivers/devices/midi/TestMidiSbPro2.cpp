#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/midi/EmulatorTestCase.hpp>
#include <HyperSonicDrivers/devices/midi/MidiSbPro2.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>

namespace HyperSonicDrivers::devices::midi
{
    TEST(MidiSbPro2, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(auto s = MidiSbPro2(mixer, op2File.getBank(), eChannelGroup::Plain));
    }

    class SbPro2Emulator_ : public EmulatorTestCase<MidiSbPro2> {};
    
    TEST_P(SbPro2Emulator_, cstr_TYPE)
    {
        test_case();
    }

    INSTANTIATE_TEST_SUITE_P(
        MidiSbPro2,
        SbPro2Emulator_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::AUTO, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplEmulator::MAME, true),
            std::make_tuple<>(OplEmulator::WOODY, true),
            std::make_tuple<>(OplEmulator::NUKED, false)
        )
    );

    TEST(MidiSbPro2, cstr_AUTO)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(MidiSbPro2(mixer, op2File.getBank(), eChannelGroup::Plain));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
