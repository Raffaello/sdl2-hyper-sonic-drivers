#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/drivers/midi/devices/SbPro.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/EmulatorTestCase.hpp>


namespace HyperSonicDrivers::drivers::midi::devices
{

    TEST(SbPro, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(auto s = SbPro(mixer, op2File.getBank(), eChannelGroup::Plain));
    }

    class SbProEmulator_ : public EmulatorTestCase<SbPro> {};
    TEST_P(SbProEmulator_, cstr_TYPE)
    {
        test_case();
    }
    INSTANTIATE_TEST_SUITE_P(
        SbPro,
        SbProEmulator_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::AUTO, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplEmulator::MAME, true),
            std::make_tuple<>(OplEmulator::WOODY, false),
            std::make_tuple<>(OplEmulator::NUKED, true)
        )
    );

    TEST(SbPro, cstr_AUTO)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(SbPro(mixer, op2File.getBank(), eChannelGroup::Plain));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
