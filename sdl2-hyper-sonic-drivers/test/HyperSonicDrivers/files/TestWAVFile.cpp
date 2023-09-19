#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <HyperSonicDrivers/audio/Sound.hpp>
#include <cstdint>
#include <memory>

namespace HyperSonicDrivers::files
{
    TEST(WAVFile, cstorDefault)
    {
        WAVFile f("../fixtures/Wav_868kb.wav", audio::mixer::eChannelGroup::Speech);

        WAVFile::format_t fmt = f.getFormat();
        EXPECT_EQ(fmt.format, WAVFile::eFormat::WAVE_FORMAT_PCM);
        EXPECT_EQ(fmt.channels, 2);
        EXPECT_EQ(fmt.samplesPerSec, 44100);
        EXPECT_EQ(fmt.avgBytesPerSec, 176400);
        EXPECT_EQ(fmt.blockAlign, 4);
        EXPECT_EQ(fmt.bitsPerSample, 16);

        uint32_t size = f.getDataSize();
        EXPECT_EQ(size, 889344);
        EXPECT_EQ(f.getData()[0], 0);
        EXPECT_EQ(f.getData()[size-1], 0);
        EXPECT_EQ(f.getData()[size/2], 0x6D);

        auto sound = f.getSound();
        EXPECT_TRUE(sound->stereo);
        EXPECT_EQ(sound->freq, fmt.samplesPerSec);
        EXPECT_EQ(16, fmt.bitsPerSample);
        EXPECT_EQ(f.getBitsDepth(), fmt.bitsPerSample);
        EXPECT_EQ(f.getChannels(), fmt.channels);
        EXPECT_EQ(sound->group, audio::mixer::eChannelGroup::Speech);
    }

    TEST(WAVFile, file_not_found)
    {
        EXPECT_THROW(WAVFile f(""), std::system_error);
    }

    TEST(WAVFile, save_empty)
    {
        {
            // TODO review group channel, especially when saving it.
            //      also it would be better passing it when getting sound probably.. anyway
            WAVFile f2("../fixtures/save_empty.wav", audio::mixer::eChannelGroup::Unknown, false);
            f2.save(44100, true, nullptr, 0);
        }

        WAVFile f2("../fixtures/save_empty.wav");
        EXPECT_EQ(f2.getSampleRate(), 44100);
        EXPECT_EQ(f2.getBitsDepth(), 16);
        EXPECT_EQ(f2.getChannels(), 2);
        EXPECT_EQ(f2.getDataSize(), 0);
        EXPECT_EQ(f2.getFormat().format, WAVFile::eFormat::WAVE_FORMAT_PCM);

    }

    TEST(WAVFile, save)
    {
        WAVFile f("../fixtures/Wav_868kb.wav", audio::mixer::eChannelGroup::Speech);
        {
            // TODO review group channel, especially when saving it.
            //      also it would be better passing it when getting sound probably.. anyway
            WAVFile f2("../fixtures/save.wav", audio::mixer::eChannelGroup::Speech, false);
            auto s = f.getSound();
            f2.save(*f.getSound());
        }

        WAVFile f2("../fixtures/save.wav");
        EXPECT_EQ(f.getBitsDepth(), f2.getBitsDepth());
        EXPECT_EQ(f.getChannels(), f2.getChannels());
        EXPECT_EQ(f.getDataSize(), f2.getDataSize());
        EXPECT_EQ(f.getDataSize(), f2.getDataSize());

    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
