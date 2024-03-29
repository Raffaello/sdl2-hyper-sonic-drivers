#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/drivers/MIDDriverMock.hpp>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/files/dmx/OP2File.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/devices/EmulatorTestCase.hpp>

namespace HyperSonicDrivers::devices
{
    TEST(Adlib, cstor_AUTO)
    {
        auto mixer = std::make_shared<IMixerMock>();
        EXPECT_NO_THROW(auto a = std::make_shared<Adlib>(mixer));
    }

    TEST(Adlib, device_name)
    {
        auto mixer = std::make_shared<IMixerMock>();
        auto a = std::make_shared<Adlib>(mixer);
        EXPECT_EQ(a->getName(), eDeviceName::Adlib);
    }

    class AdliblEmulator_ : public EmulatorTestCase<Adlib> {};
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
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
