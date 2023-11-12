#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/utils/sound.hpp>
#include <HyperSonicDrivers/audio/PCMSound.hpp>

namespace HyperSonicDrivers::utils
{
    using audio::mixer::eChannelGroup;
    using audio::PCMSound;

    TEST(utils, makeMono0)
    {
        const uint32_t size = 100;
        auto data = std::make_shared<int16_t[]>(size);

        auto s1 = std::make_shared<PCMSound>(eChannelGroup::Plain, true, 44100, size, data);
        auto s2 = makeMono(s1);

        EXPECT_TRUE(s1->stereo);
        EXPECT_FALSE(s2->stereo);
        EXPECT_EQ(s1->group, s2->group);
        EXPECT_EQ(s1->freq, s2->freq);
        EXPECT_EQ(s1->dataSize, size);
        EXPECT_EQ(s2->dataSize, size / 2);
    }

    TEST(utils, makeMono1)
    {
        const uint32_t size = 100;
        auto data = std::make_shared<int16_t[]>(size);

        auto s1 = std::make_shared<PCMSound>(eChannelGroup::Plain, false, 44100, size, data);
        auto s2 = makeMono(s1);

        ASSERT_EQ(s1, s2);
    }

    TEST(utils, makeStereo0)
    {
        const uint32_t size = 100;
        auto data = std::make_shared<int16_t[]>(size);

        auto s1 = std::make_shared<PCMSound>(eChannelGroup::Plain, false, 44100, size, data);
        auto s2 = makeStereo(s1);

        EXPECT_FALSE(s1->stereo);
        EXPECT_TRUE(s2->stereo);
        EXPECT_EQ(s1->group, s2->group);
        EXPECT_EQ(s1->freq, s2->freq);
        EXPECT_EQ(s1->dataSize, size);
        EXPECT_EQ(s2->dataSize, size * 2);
    }

    TEST(utils, makeStereo1)
    {
        const uint32_t size = 100;
        auto data = std::make_shared<int16_t[]>(size);

        auto s1 = std::make_shared<PCMSound>(eChannelGroup::Plain, true, 44100, size, data);
        auto s2 = makeStereo(s1);

        ASSERT_EQ(s1, s2);
    }

    TEST(utils, duration_ms)
    {
        const uint32_t size = 44100 * 2;
        auto data = std::make_shared<int16_t[]>(size);

        auto s1 = std::make_shared<PCMSound>(eChannelGroup::Plain, true, 44100, size, data);
        auto s2 = std::make_shared<PCMSound>(eChannelGroup::Plain, false, 44100, size, data);
        auto s3 = std::make_shared<PCMSound>(eChannelGroup::Plain, false, 44100, 11025 * 7, data);
        auto s4 = std::make_shared<PCMSound>(eChannelGroup::Plain, false, 8000, size, data);
        auto s5 = std::make_shared<PCMSound>(eChannelGroup::Plain, true, 8000, size, data);

        EXPECT_EQ(duration_ms(s1), 1000);
        EXPECT_EQ(duration_ms(s2), 2000);
        EXPECT_EQ(duration_ms(s3), 1750);
        EXPECT_EQ(duration_ms(s4), 11025);
        EXPECT_EQ(duration_ms(s5), 5512);
    }

    TEST(utils, ms_toPos)
    {
        const uint32_t size = 44100 * 2;
        auto data = std::make_shared<int16_t[]>(size);

        auto s1 = std::make_shared<PCMSound>(eChannelGroup::Plain, true, 44100, size, data);

        EXPECT_EQ(ms_toPos(0, s1), 0);
        EXPECT_EQ(ms_toPos(1000, s1), 44100 * 2);
        EXPECT_EQ(ms_toPos(500, s1), 22050 * 2);
        EXPECT_EQ(ms_toPos(250, s1), 11025 * 2);
        EXPECT_EQ(ms_toPos(100, s1), 4410 * 2);

        auto s2 = std::make_shared<PCMSound>(eChannelGroup::Plain, false, 44100, size, data);

        EXPECT_EQ(ms_toPos(0, s2), 0);
        EXPECT_EQ(ms_toPos(1000, s2), 44100);
        EXPECT_EQ(ms_toPos(500, s2), 22050);
        EXPECT_EQ(ms_toPos(250, s2), 11025);
        EXPECT_EQ(ms_toPos(100, s2), 4410);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
