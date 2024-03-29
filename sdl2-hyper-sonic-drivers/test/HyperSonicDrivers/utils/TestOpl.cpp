#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/utils/opl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>
#include <memory>

namespace HyperSonicDrivers::utils
{
    // NOTE: Disabled due to CI Linux and Mac

    using hardware::opl::OPL;
    using hardware::opl::OPLFactory;
    using hardware::opl::OplEmulator;
    using hardware::opl::OplType;
    using audio::IMixerMock;

    class OplType_ : public ::testing::TestWithParam<std::tuple<OplEmulator, OplType, bool, bool>>
    {
    public:
        std::shared_ptr<IMixerMock> mixer = std::make_shared<IMixerMock>();
        OplEmulator opl_emu = std::get<0>(GetParam());
        OplType opl_type = std::get<1>(GetParam());
        bool isOpl2 = std::get<2>(GetParam());
        bool isOpl3 = std::get<3>(GetParam());
        
        std::shared_ptr<OPL> opl = OPLFactory::create(opl_emu, opl_type, mixer);
    };
    TEST_P(OplType_, DetectOPL)
    {
        ASSERT_TRUE(this->opl->init());
        this->opl->start(nullptr);
        EXPECT_EQ(detectOPL2(this->opl), isOpl2);
        EXPECT_EQ(detectOPL3(this->opl), isOpl3);
    }
    INSTANTIATE_TEST_SUITE_P(
        DetectOpl,
        OplType_,
        ::testing::Values(
            std::make_tuple<>(OplEmulator::MAME, OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::DOS_BOX, OplType::OPL3, true, true),
            std::make_tuple<>(OplEmulator::AUTO, OplType::OPL2, true, false),
            std::make_tuple<>(OplEmulator::AUTO, OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::AUTO, OplType::OPL3, true, true),
            //std::make_tuple<>(OplEmulator::NUKED, OplType::OPL2, true, false)
            //std::make_tuple<>(OplEmulator::NUKED, OplType::DUAL_OPL2, true, false),
            std::make_tuple<>(OplEmulator::NUKED, OplType::OPL3, true, true)
            //std::make_tuple<>(OplEmulator::WOODY, OplType::OPL2, true, false),
            //std::make_tuple<>(OplEmulator::WOODY, OplType::DUAL_OPL2, true, false)
        )
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
