#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/drivers/midi/devices/SbPro.hpp>
#include <HyperSonicDrivers/drivers/MIDDriverMock.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/files/dmx/OP2File.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    using audio::stubs::StubMixer;
    using hardware::opl::OplType;
    using hardware::opl::OplEmulator;
    using hardware::opl::OPLFactory;
    using files::dmx::OP2File;

    const std::string GENMIDI_OP2 = std::string("../fixtures/GENMIDI.OP2");

    TEST(SbPro, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(auto s = SbPro(mixer, op2File.getBank()));
    }

    class SbPro2Emulator_ : public ::testing::TestWithParam<std::tuple<hardware::opl::OplEmulator, bool>>
    {
    public:
        const OplEmulator oplEmu = std::get<0>(GetParam());
        const bool shouldThrow = std::get<1>(GetParam());
        const OP2File op2File = OP2File(GENMIDI_OP2);
        const std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();

    };
    TEST_P(SbPro2Emulator_, cstr_TYPE)
    {
        if (this->shouldThrow) {
            EXPECT_THROW(
                devices::SbPro(this->mixer, this->op2File.getBank(), this->oplEmu),
                std::runtime_error
            );
        }
        else {
            EXPECT_NO_THROW(devices::SbPro(this->mixer, this->op2File.getBank(), this->oplEmu));
        }
    }
    INSTANTIATE_TEST_SUITE_P(
        SbPro,
        SbPro2Emulator_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::AUTO, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplEmulator::MAME, true),
            std::make_tuple<>(OplEmulator::WOODY, false),
            std::make_tuple<>(OplEmulator::NUKED, false)
        )
    );

    TEST(SbPro, cstr_AUTO)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        EXPECT_NO_THROW(SbPro(mixer, op2File.getBank()));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
