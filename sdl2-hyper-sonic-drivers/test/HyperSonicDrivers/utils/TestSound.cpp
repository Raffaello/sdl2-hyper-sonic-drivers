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
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
