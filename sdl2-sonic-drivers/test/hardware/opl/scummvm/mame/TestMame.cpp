#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <audio/SDL2Mixer.hpp>
#include <memory>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            namespace mame
            {
                TEST(OPL, cstorDefault)
                {
                    int rate = 44100;
                    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
                    EXPECT_EQ(mixer.use_count(), 1);
                    OPL mame(mixer);
                    EXPECT_EQ(mixer.use_count(), 2);
                }

                TEST(OPL, share_ptrDefault)
                {
                    int rate = 44100;
                    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
                    EXPECT_EQ(mixer.use_count(), 1);
                    std::shared_ptr<OPL> mame = std::make_shared<OPL>(mixer);
                    EXPECT_EQ(mixer.use_count(), 2);
                    EXPECT_EQ(mame.use_count(), 1);
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
