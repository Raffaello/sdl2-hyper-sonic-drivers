#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/MIDFile.hpp>
#include <audio/midi/types.hpp>
#include <audio/MIDI.hpp>
#include <audio/midi/MIDITrack.hpp>
#include <array>
#include <cstdint>

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

    void cmp_midievent(const audio::midi::MIDIEvent& e, const uint32_t exp_abs_time, const uint32_t exp_delta_time, const std::vector<uint8_t>& exp_data)
    {
        EXPECT_EQ(e.abs_time, exp_abs_time);
        EXPECT_EQ(e.delta_time, exp_delta_time);

        EXPECT_EQ(e.data.size(), exp_data.size());
        for (int i = 0; i < e.data.size(); i++)
            EXPECT_EQ(e.data[i], exp_data[i]);
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
        
        const std::array<uint32_t, 61> exp_abs_times = {
            0, 0, 0, 120, 120, 120, 120, 240, 240, 240, 240, 360, 360, 360,
            360, 480, 480, 480, 480, 600, 600, 600, 600, 720, 720, 720, 720, 840, 840,
            960,  960, 960, 960, 1080, 1080, 1080, 1080, 1200,1200,1200,1200,1320,1320,
            1320,1320,1440,1440,1440,1440,1560,1560,1560,1560,1680,1680,1680,1680,1920,
            1920,1920,1920 };

        const std::array<uint32_t, 1 + 29 + 31> exp_delta_times = {
            0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,120,0,120,
            0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,120,0,0,0,240,0,0,0
        };

        std::vector<std::vector<uint8_t>> v = { { 1,2,3 } };

        const std::vector<std::vector<uint8_t>> exp_datas = {
            {0x90, 0x48, 0x40},{0x90, 0x30, 0x40},{0xff, 0x2f, 0},{0x80, 0x48, 0x40},
{0x80, 0x30, 0x40},{0x90, 0x3c, 0x40},{0x90, 0x48, 0x40},{0x80, 0x3c, 0x40},{0x80, 0x48, 0x40},
{0x90, 0x40, 0x40},{0x90, 0x4f, 0x40},{0x80, 0x40, 0x40},{0x80, 0x4f, 0x40},{0x90, 0x4f, 0x40},
{0x90, 0x3c, 0x40},{0x80, 0x3c, 0x40},{0x80, 0x4f, 0x40},{0x90, 0x41, 0x40},{0x90, 0x51, 0x40},
{0x80, 0x41, 0x40},{0x80, 0x51, 0x40},{0x90, 0x51, 0x40},{0x90, 0x3c, 0x40},{0x80, 0x51, 0x40},
{0x80, 0x3c, 0x40},{0x90, 0x40, 0x40},{0x90, 0x4f, 0x40},{0x80, 0x40, 0x40},{0x90, 0x3c, 0x40},
{0x80, 0x3c, 0x40},{0x80, 0x4f, 0x40},{0x90, 0x3e, 0x40},{0x90, 0x4d, 0x40},{0x80, 0x4d, 0x40},
{0x80, 0x3e, 0x40},{0x90, 0x3b, 0x40},{0x90, 0x4d, 0x40},{0x80, 0x4d, 0x40},{0x80, 0x3b, 0x40},
{0x90, 0x3c, 0x40},{0x90, 0x4c, 0x40},{0x80, 0x4c, 0x40},{0x80, 0x3c, 0x40},{0x90, 0x4c, 0x40},
{0x90, 0x39, 0x40},{0x80, 0x4c, 0x40},{0x80, 0x39, 0x40},{0x90, 0x4a, 0x40},{0x90, 0x35, 0x40},
{0x80, 0x4a, 0x40},{0x80, 0x35, 0x40},{0x90, 0x37, 0x40},{0x90, 0x4a, 0x40},{0x80, 0x4a, 0x40},
{0x80, 0x37, 0x40},{0x90, 0x30, 0x40},{0x90, 0x48, 0x40},{0x80, 0x48, 0x40},{0x80, 0x30, 0x40},
{0xff, 0x2f, 0},{0xff, 0x2f, 0}
        };

        auto t = m->getTrack(0);

        for (int i = 0; i < 61; i++) {
            cmp_midievent(t.events[i], exp_abs_times[i], exp_delta_times[i], exp_datas[i]);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
