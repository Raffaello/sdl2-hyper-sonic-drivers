#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/dmx/MUSFile.hpp>

namespace files
{
    namespace dmx
    {
        TEST(MUSFile, cstorDefault)
        {
            MUSFile f("fixtures/D_E1M1.MUS");

            auto midi = f.getMIDI();
            EXPECT_EQ(midi->format, audio::midi::MIDI_FORMAT::SINGLE_TRACK);
            EXPECT_EQ(midi->numTracks, 1);
            EXPECT_EQ(midi->division, MUSFile::MUS_PLAYBACK_SPEED_DEFAULT);
            auto te = midi->getTrack().getEvents();

            EXPECT_EQ(te.size(), 937);
        }

        TEST(MUSFile, file_not_found)
        {
            EXPECT_THROW(MUSFile f(""), std::system_error);
        }

        /*TEST(MUSFile, musfile_sample)
        {
            MUSFile f("fixtures/D_E1M1.MUS");
            
            
        }*/

        class TestMUSPitch : public ::testing::TestWithParam<std::tuple<uint8_t, uint16_t>> {};
        TEST_P(TestMUSPitch, pitch_interpolation)
        {
            uint8_t pitch = std::get<0>(GetParam());
            uint16_t midi_pitch = std::get<1>(GetParam());

            // formula
            EXPECT_EQ(midi_pitch, pitch * 64);
            EXPECT_EQ(midi_pitch, pitch << 6);
        }
        INSTANTIATE_TEST_SUITE_P(
            DISABLED_MUSFile,
            TestMUSPitch,
            ::testing::Values(
                std::make_tuple<>(0,  0),
                std::make_tuple<>(64,  4096),
                std::make_tuple<>(128, 8192),
                std::make_tuple<>(192, 12288),
                std::make_tuple<>(255, 16383)
            )
        );
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
