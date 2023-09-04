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
        WAVFile f("../fixtures/Wav_868kb.wav", audio::scummvm::Mixer::SoundType::SPEECH);

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
        EXPECT_EQ(sound->rate, fmt.samplesPerSec);
        EXPECT_EQ(sound->bitsDepth, fmt.bitsPerSample);
        EXPECT_EQ(sound->soundType, audio::scummvm::Mixer::SoundType::SPEECH);
    }

    TEST(WAVFile, file_not_found)
    {
        EXPECT_THROW(WAVFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}