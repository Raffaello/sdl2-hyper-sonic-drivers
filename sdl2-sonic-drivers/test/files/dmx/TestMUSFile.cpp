#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/dmx/MUSFile.hpp>
#include <files/MIDFile.hpp>

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
            EXPECT_EQ(midi->division, MUSFile::MUS_PLAYBACK_SPEED_DEFAULT / 2);
            auto te = midi->getTrack().getEvents();

            EXPECT_EQ(te.size(), 5828);
        }

        TEST(MUSFile, file_not_found)
        {
            EXPECT_THROW(MUSFile f(""), std::system_error);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
