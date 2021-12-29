#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/MIDFile.hpp>
#include <audio/midi/types.hpp>
#include <audio/MIDI.hpp>
#include <audio/midi/MIDITrack.hpp>

namespace files
{
    TEST(MIDFile, cstorDefault)
    {
        MIDFile f("fixtures/MI_intro.mid");
        
        EXPECT_EQ(f.getMIDI()->format, audio::midi::MIDI_FORMAT::SIMULTANEOUS_TRACK);
        EXPECT_EQ(f.getMIDI()->numTracks, 15);
        EXPECT_EQ(f.getMIDI()->division, 192);

        auto track0 = f.getMIDI()->getTrack(0);
        EXPECT_EQ(track0.events.size(), 4);
        EXPECT_EQ(track0.events[3].type.val, 0xFF);
        EXPECT_EQ(track0.events[3].data.size(), 1);
        EXPECT_EQ(track0.events[3].data[0], (int)audio::midi::MIDI_META_EVENT::END_OF_TRACK);
        EXPECT_EQ(track0.events[3].delta_time, 0);
    }

    TEST(MIDFile, file_not_found)
    {
        EXPECT_THROW(MIDFile f(""), std::system_error);
    }

    TEST(MIDFile, midifile_sample)
    {
        MIDFile f("fixtures/midifile_sample.mid");

    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
