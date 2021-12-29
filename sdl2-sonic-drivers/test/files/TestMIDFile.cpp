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

        auto m = f.getMIDI();
        
        EXPECT_EQ(m->format, audio::midi::MIDI_FORMAT::SIMULTANEOUS_TRACK);
        EXPECT_EQ(m->numTracks, 3);
        EXPECT_EQ(m->division, 120);
        
        EXPECT_EQ(m->getTrack(0).events.size(), 1);
        EXPECT_EQ(m->getTrack(1).events.size(), 29);
        EXPECT_EQ(m->getTrack(2).events.size(), 31);

        // absolute time checks
        auto t = m->getTrack(0);
        EXPECT_EQ(t.events[0].abs_time, 0);
        
        t = m->getTrack(1);
        EXPECT_EQ(t.events[0].abs_time, 0);
        EXPECT_EQ(t.events[1].abs_time, 120);
        EXPECT_EQ(t.events[2].abs_time, 120);
        EXPECT_EQ(t.events[3].abs_time, 240);
        // ... 
        EXPECT_EQ(t.events[28].abs_time, 1920);

        t = m->getTrack(2);
        EXPECT_EQ(t.events[0].abs_time, 0);
        EXPECT_EQ(t.events[1].abs_time, 120);
        EXPECT_EQ(t.events[2].abs_time, 120);
        EXPECT_EQ(t.events[3].abs_time, 240);
        // ... 
        EXPECT_EQ(t.events[30].abs_time, 1920);
    }

    TEST(MIDFile, midifile_sample_convert_to_single_track)
    {
        MIDFile f("fixtures/midifile_sample.mid");

        // todo
        auto m = f.convertToSingleTrackMIDI();

        EXPECT_EQ(m->format, audio::midi::MIDI_FORMAT::SINGLE_TRACK);
        EXPECT_EQ(m->numTracks, 1);
        EXPECT_EQ(m->division, 120);
        EXPECT_EQ(m->getTrack(0).events.size(), 1 + 29 + 31);
        
        auto t = m->getTrack(0);
        EXPECT_EQ(t.events[0].abs_time, 0);
        EXPECT_EQ(t.events[1].abs_time, 0);
        EXPECT_EQ(t.events[2].abs_time, 0);
        EXPECT_EQ(t.events[3].abs_time, 120);
        EXPECT_EQ(t.events[4].abs_time, 120);
        EXPECT_EQ(t.events[5].abs_time, 120);
        EXPECT_EQ(t.events[6].abs_time, 120);
        EXPECT_EQ(t.events[7].abs_time, 240);

        // check delta times
        EXPECT_EQ(t.events[0].delta_time, 0);
        EXPECT_EQ(t.events[1].delta_time, 0);
        EXPECT_EQ(t.events[2].delta_time, 0);
        EXPECT_EQ(t.events[3].delta_time, 120);
        EXPECT_EQ(t.events[4].delta_time, 0);
        EXPECT_EQ(t.events[5].delta_time, 0);
        EXPECT_EQ(t.events[6].delta_time, 0);
        EXPECT_EQ(t.events[7].delta_time, 120);
        // ...
        EXPECT_EQ(t.events[57].delta_time, 240);
        EXPECT_EQ(t.events[58].delta_time, 0);
        EXPECT_EQ(t.events[59].delta_time, 0);
        EXPECT_EQ(t.events[60].delta_time, 0);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
