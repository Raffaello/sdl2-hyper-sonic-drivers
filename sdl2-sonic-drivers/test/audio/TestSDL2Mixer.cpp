#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <audio/SDL2Mixer.hpp>
#include <memory>

#include <audio/SDL2Mixer.hpp>

namespace audio
{
    TEST(SDL2Mixer, cstorDefault)
    {
        SDL2Mixer mixer();
    }

    TEST(SDL2Mixer, share_ptrDefault)
    {
        std::shared_ptr<SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
