#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drivers/midi/devices/Opl.hpp>
#include <drivers/MIDDriverMock.hpp>
#include <audio/stubs/StubMixer.hpp>
#include <hardware/opl/OplType.hpp>
#include <files/dmx/OP2File.hpp>
#include <hardware/opl/Config.hpp>
#include <hardware/opl/OPL.hpp>
#include <audio/opl/banks/OP2Bank.hpp>
#include <audio/scummvm/Mixer.hpp>

namespace drivers::midi::devices
{
    using audio::stubs::StubMixer;
    using audio::opl::banks::OP2Bank;
    using hardware::opl::OplType;
    using hardware::opl::OplEmulator;
    using hardware::opl::Config;
    using hardware::opl::OPL;
    using files::dmx::OP2File;
    using files::dmx::OP2File;

    const std::string GENMIDI_OP2 = std::string("fixtures/GENMIDI.OP2");

    class OplMock : public Opl
    {
    public:
        explicit OplMock(const std::shared_ptr<OPL>& opl, const std::shared_ptr<OP2Bank>& op2Bank)
            : Opl(opl, op2Bank) {}
        explicit OplMock(const OplType type,
            const OplEmulator emuType,
            const std::shared_ptr<audio::scummvm::Mixer>& mixer,
            const std::shared_ptr<OP2Bank>& op2Bank)
            : Opl(type, emuType, mixer, op2Bank) {}
        ~OplMock() override = default;
    };

    TEST(Opl, cstor_)
    {
        auto op2File = OP2File(GENMIDI_OP2);
        auto mixer = std::make_shared<StubMixer>();
        auto opl = Config::create(OplEmulator::AUTO, OplType::OPL2, mixer);
        EXPECT_NO_THROW(std::make_shared<OplMock>(opl, op2File.getBank()));
        opl = nullptr;
        EXPECT_THROW(std::make_shared<OplMock>(opl, op2File.getBank()), std::runtime_error);
    }

    class OplEmulator_ : public ::testing::TestWithParam<std::tuple<OplType, OplEmulator, bool>>
    {
    public:
        const OplType oplType = std::get<0>(GetParam());
        const OplEmulator oplEmu = std::get<1>(GetParam());
        const bool shouldThrow = std::get<2>(GetParam());
        const OP2File op2File = OP2File(GENMIDI_OP2);
        const std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();

    };
    TEST_P(OplEmulator_, cstr_type_emu)
    {
        if (this->shouldThrow) {
            EXPECT_THROW(
                std::make_shared<devices::OplMock>(this->oplType, this->oplEmu, this->mixer, this->op2File.getBank()),
                std::runtime_error
            );
        }
        else {
            EXPECT_NO_THROW(std::make_shared<devices::OplMock>(this->oplType, this->oplEmu, this->mixer, this->op2File.getBank()));
        }
    }

    INSTANTIATE_TEST_SUITE_P(
        Opl,
        OplEmulator_,
        ::testing::Values(
            std::make_tuple<>(OplType::OPL2, OplEmulator::AUTO, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::AUTO, false),
            std::make_tuple<>(OplType::OPL3, OplEmulator::AUTO, false),
            std::make_tuple<>(OplType::OPL2, OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplType::OPL3, OplEmulator::DOS_BOX, false),
            std::make_tuple<>(OplType::OPL2, OplEmulator::MAME, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::MAME, true),
            std::make_tuple<>(OplType::OPL3, OplEmulator::MAME, true),
            std::make_tuple<>(OplType::OPL2, OplEmulator::WOODY, false),
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::WOODY, false),
            std::make_tuple<>(OplType::OPL3, OplEmulator::WOODY, true),
            std::make_tuple<>(OplType::OPL2, OplEmulator::NUKED, true), // TODO: this is OPL2 compatible so, it should work, at the moment forced only to be OPL3 type
            std::make_tuple<>(OplType::DUAL_OPL2, OplEmulator::NUKED, true), // TODO: this is OPL2 compatible so, it should work, at the moment forced only to be OPL3 type
            std::make_tuple<>(OplType::OPL3, OplEmulator::NUKED, false)
        )
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
