#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/midi/MidiSbPro.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/devices/midi/EmulatorTestCase.hpp>


namespace HyperSonicDrivers::devices::midi
{

    TEST(MidiSbPro, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(auto s = MidiSbPro(mixer, op2File.getBank(), eChannelGroup::Plain));
    }

    class SbProEmulator_ : public EmulatorTestCase<MidiSbPro> {};
    TEST_P(SbProEmulator_, cstr_TYPE)
    {
        test_case();
    }
    INSTANTIATE_TEST_SUITE_P(
        MidiSbPro,
        SbProEmulator_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::AUTO, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplEmulator::MAME, true),
            std::make_tuple<>(OplEmulator::WOODY, false),
            std::make_tuple<>(OplEmulator::NUKED, true)
        )
    );

    TEST(MidiSbPro, cstr_AUTO)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(MidiSbPro(mixer, op2File.getBank(), eChannelGroup::Plain));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
