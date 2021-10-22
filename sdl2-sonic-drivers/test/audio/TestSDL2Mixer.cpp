#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <audio/SDL2Mixer.hpp>
#include <memory>

#include <audio/SDL2Mixer.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

namespace audio
{
    TEST(SDL2Mixer, SDL2_not_init)
    {
        EXPECT_THROW(audio::SDL2Mixer mixer, std::runtime_error);
    }

    TEST(SDL2Mixer, SDL2_audio_not_init)
    {
        ASSERT_EQ(SDL_Init(SDL_INIT_AUDIO), 0);
        EXPECT_THROW(audio::SDL2Mixer mixer, std::runtime_error);

        SDL_Quit();
    }

    TEST(SDL2Mixer, cstorDefault)
    {
        int rate = 44100;

        // TODO: use setUp and tearDown to for SDL_Init and Mix_OpenAudio
        ASSERT_EQ(SDL_Init(SDL_INIT_AUDIO), 0);
        int res = Mix_OpenAudio(rate, AUDIO_S16, 2, 1024);
        if (res == -1) {
            GTEST_SKIP() << "Cannot open Audio device";
        }

        SDL2Mixer mixer;

        EXPECT_EQ(mixer.getOutputRate(), rate);

        Mix_Quit();
        SDL_Quit();
    }

    TEST(SDL2Mixer, share_ptrDefault)
    {
        int rate = 44100;

        ASSERT_EQ(SDL_Init(SDL_INIT_AUDIO), 0);
        int res = Mix_OpenAudio(rate, AUDIO_S16, 2, 1024);
        if (res == -1) {
            GTEST_SKIP() << "Cannot open Audio device";
        }
        
        std::shared_ptr<SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
        EXPECT_EQ(mixer.use_count(), 1);
        EXPECT_EQ(mixer->getOutputRate(), rate);

        Mix_Quit();
        SDL_Quit();
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
