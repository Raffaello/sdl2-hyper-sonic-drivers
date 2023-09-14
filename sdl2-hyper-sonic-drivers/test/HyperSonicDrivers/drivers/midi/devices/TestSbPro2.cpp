#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/drivers/midi/devices/EmulatorTestCase.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/SbPro2.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    TEST(SbPro2, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(auto s = SbPro2(mixer, op2File.getBank(), eChannelGroup::Plain));
    }

    class SbPro2Emulator_ : public EmulatorTestCase<SbPro2> {};
    
    TEST_P(SbPro2Emulator_, cstr_TYPE)
    {
        test_case();
    }

    INSTANTIATE_TEST_SUITE_P(
        SbPro2,
        SbPro2Emulator_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::AUTO, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplEmulator::MAME, true),
            std::make_tuple<>(OplEmulator::WOODY, true),
            std::make_tuple<>(OplEmulator::NUKED, false)
        )
    );

    TEST(SbPro2, cstr_AUTO)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(SbPro2(mixer, op2File.getBank(), eChannelGroup::Plain));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
