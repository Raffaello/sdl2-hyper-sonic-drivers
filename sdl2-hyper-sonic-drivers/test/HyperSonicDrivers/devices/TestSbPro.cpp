#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/SbPro.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/devices/EmulatorTestCase.hpp>


namespace HyperSonicDrivers::devices
{

    TEST(SbPro, cstor_)
    {
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(auto s = SbPro(mixer, OplEmulator::DOS_BOX));
    }

    TEST(Adlib, device_name)
    {
        auto mixer = std::make_shared<StubMixer>();
        auto s = std::make_shared<SbPro>(mixer);
        EXPECT_EQ(s->getName(), eDeviceName::SbPro);
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
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(SbPro(mixer, OplEmulator::AUTO));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
