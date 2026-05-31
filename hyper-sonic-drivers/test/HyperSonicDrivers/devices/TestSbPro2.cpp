#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/EmulatorTestCase.hpp>
#include <HyperSonicDrivers/devices/SbPro2.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>

namespace HyperSonicDrivers::devices
{
    TEST(SbPro2, cstor_)
    {
        auto mixer = std::make_shared<IMixerMock>();
        EXPECT_NO_THROW(auto s = SbPro2(mixer, OplEmulator::NUKED));
    }

    TEST(Adlib, device_name)
    {
        auto mixer = std::make_shared<IMixerMock>();
        auto s = std::make_shared<SbPro2>(mixer);
        EXPECT_EQ(s->getName(), eDeviceName::SbPro2);
    }

    class SbPro2Emulator_ : public EmulatorTestCase<SbPro2> {};
    
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

    TEST(SbPro2, cstor_AUTO)
    {
        auto mixer = std::make_shared<IMixerMock>();
        EXPECT_NO_THROW(auto s = SbPro2(mixer));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
