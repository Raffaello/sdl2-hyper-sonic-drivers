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

    class SDL2MixerTest : public ::testing::Test
    {
    public:
        const int rate = 44100;
    protected:
        void SetUp() override
        {
            if ((SDL_Init(SDL_INIT_AUDIO) != 0) || 
                (Mix_OpenAudio(this->rate, AUDIO_S16, 2, 1024) != 0)) {
                GTEST_SKIP() << "Cannot open Audio device";
            }

        }

        void TearDown() override
        {
            Mix_CloseAudio();
            Mix_Quit();
            SDL_Quit();
        }
    };

    TEST_F(SDL2MixerTest, cstorDefault)
    {
        SDL2Mixer mixer;

        EXPECT_EQ(mixer.getOutputRate(), this->rate);
    }

    TEST_F(SDL2MixerTest, share_ptrDefault)
    {
        std::shared_ptr<SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
        EXPECT_EQ(mixer.use_count(), 1);
        EXPECT_EQ(mixer->getOutputRate(), this->rate);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
