#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/drivers/PCMDriver.hpp>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>
#include <HyperSonicDrivers/audio/PCMSound.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <memory>
#include <cstdint>

namespace HyperSonicDrivers::drivers
{
    TEST(PCMDriver, play_stop0)
    {
        auto mixer = audio::make_mixer<audio::IMixerMock>();
        auto drv = PCMDriver(mixer);

        EXPECT_EQ(drv.max_streams, mixer->max_channels);

        auto sound_data = std::make_shared<int16_t[]>(1);
        auto sound = std::make_shared<audio::PCMSound>(audio::mixer::eChannelGroup::Plain, true, 44100, 1, sound_data);
        auto ch_id = drv.play(sound);

        ASSERT_TRUE(drv.isPlaying(sound));
        ASSERT_TRUE(drv.isPlaying());
        ASSERT_TRUE(ch_id.has_value());
        EXPECT_EQ(ch_id.value(), 0);

        drv.stop(ch_id.value());
        EXPECT_FALSE(drv.isPlaying(sound));
        EXPECT_FALSE(drv.isPlaying());
    }

    TEST(PCMDriver, play_stop1)
    {
        auto mixer = audio::make_mixer<audio::IMixerMock>();
        auto drv = PCMDriver(mixer);

        EXPECT_EQ(drv.max_streams, mixer->max_channels);

        auto sound_data = std::make_shared<int16_t[]>(1);
        auto sound = std::make_shared<audio::PCMSound>(audio::mixer::eChannelGroup::Plain, true, 44100, 1, sound_data);
        auto ch_id = drv.play(sound);

        ASSERT_TRUE(drv.isPlaying(sound));
        ASSERT_TRUE(drv.isPlaying());
        ASSERT_TRUE(ch_id.has_value());
        EXPECT_EQ(ch_id.value(), 0);

        drv.stop(sound);
        EXPECT_FALSE(drv.isPlaying(sound));
        EXPECT_FALSE(drv.isPlaying());
    }

    TEST(PCMDriver, play_stop2)
    {
        auto mixer = audio::make_mixer<audio::IMixerMock>();
        auto drv = PCMDriver(mixer);

        EXPECT_EQ(drv.max_streams, mixer->max_channels);

        auto sound_data = std::make_shared<int16_t[]>(1);
        auto sound = std::make_shared<audio::PCMSound>(audio::mixer::eChannelGroup::Plain, true, 44100, 1, sound_data);
        auto ch_id = drv.play(sound);

        ASSERT_TRUE(drv.isPlaying(sound));
        ASSERT_TRUE(drv.isPlaying());
        ASSERT_TRUE(ch_id.has_value());
        EXPECT_EQ(ch_id.value(), 0);

        drv.stop();
        EXPECT_FALSE(drv.isPlaying(sound));
        EXPECT_FALSE(drv.isPlaying());
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
