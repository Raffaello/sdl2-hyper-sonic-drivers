#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drivers/midi/devices/SbPro2.hpp>
#include <drivers/MIDDriverMock.hpp>
#include <audio/stubs/StubMixer.hpp>
#include <hardware/opl/OplEmulator.hpp>
#include <hardware/opl/OplType.hpp>
#include <files/dmx/OP2File.hpp>
#include <hardware/opl/OPLFactory.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    using audio::stubs::StubMixer;
    using hardware::opl::OplType;
    using hardware::opl::OplEmulator;
    using hardware::opl::OPLFactory;
    using files::dmx::OP2File;

    const std::string GENMIDI_OP2 = std::string("fixtures/GENMIDI.OP2");

    TEST(Adlib, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        auto opl = OPLFactory::create(OplEmulator::AUTO, OplType::OPL2, mixer);
        EXPECT_NO_THROW(std::make_shared<SbPro2>(opl, op2File.getBank()));
        opl = nullptr;
        EXPECT_THROW(std::make_shared<SbPro2>(opl, op2File.getBank()), std::runtime_error);
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
                std::make_shared<devices::SbPro2>(this->oplEmu, this->mixer, this->op2File.getBank()),
                std::runtime_error
            );
        }
        else {
            EXPECT_NO_THROW(std::make_shared<devices::SbPro2>(this->oplEmu, this->mixer, this->op2File.getBank()));
        }
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
        EXPECT_NO_THROW(std::make_shared<SbPro2>(mixer, op2File.getBank()));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
