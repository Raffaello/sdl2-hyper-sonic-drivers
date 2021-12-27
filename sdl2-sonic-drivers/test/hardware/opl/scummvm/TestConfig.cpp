#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/scummvm/Config.hpp>
#include "../test/audio/mocks/MockMixer.hpp"
#include <hardware/opl/woody/WoodyOPL.hpp>
#include <hardware/opl/scummvm/dosbox/dosbox.hpp>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <hardware/opl/scummvm/nuked/OPL.hpp>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            using audio::mocks::MockMixer;

            const static std::shared_ptr<MockMixer> mixer = std::make_shared<MockMixer>();

            class ConfigTest : public ::testing::TestWithParam<std::tuple<OplEmulator, Config::OplType>>{};
            TEST_P(ConfigTest, create)
            {
                OplEmulator emu = std::get<0>(GetParam());
                Config::OplType type = std::get<1>(GetParam());
                
                auto opl = Config::create(emu, type, mixer);
                EXPECT_TRUE(opl->init());
                EXPECT_NE(opl, nullptr);
                EXPECT_EQ(opl.use_count(), 1);
                EmulatedOPL* w = nullptr;
                switch (emu)
                {
                case OplEmulator::MAME:
                    EXPECT_NO_THROW(w = dynamic_cast<mame::OPL*>(opl.get()));
                    break;
                case OplEmulator::DOS_BOX:
                case OplEmulator::AUTO:
                    EXPECT_NO_THROW(w = dynamic_cast<dosbox::OPL*>(opl.get()));
                    break;
                case OplEmulator::NUKED:
                    EXPECT_NO_THROW(w = dynamic_cast<nuked::OPL*>(opl.get()));
                    break;
                case OplEmulator::WOODY:
                    EXPECT_NO_THROW(w = dynamic_cast<woody::WoodyOPL*>(opl.get()));
                    break;
                default:
                    FAIL();
                }

                EXPECT_NE(w, nullptr);
            }
            INSTANTIATE_TEST_SUITE_P(
                Config,
                ConfigTest,
                ::testing::Values(
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::MAME, Config::OplType::OPL2),
                    //std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::MAME, Config::OplType::DUAL_OPL2),
                    //std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::MAME, Config::OplType::OPL3),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::DOS_BOX, Config::OplType::OPL2),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::DOS_BOX, Config::OplType::DUAL_OPL2),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::DOS_BOX, Config::OplType::OPL3),
                    //std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::NUKED, Config::OplType::OPL2),
                    //std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::NUKED, Config::OplType::DUAL_OPL2),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::NUKED, Config::OplType::OPL3),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::WOODY, Config::OplType::OPL2),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::WOODY, Config::OplType::DUAL_OPL2),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::WOODY, Config::OplType::OPL3),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::AUTO, Config::OplType::OPL2),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::AUTO, Config::OplType::DUAL_OPL2),
                    std::make_tuple<OplEmulator, Config::OplType>(OplEmulator::AUTO, Config::OplType::OPL3)
                )
            );
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
