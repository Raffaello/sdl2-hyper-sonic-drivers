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
        EXPECT_EQ(track0.getEvents().size(), 4);
        EXPECT_EQ(track0.getEvents()[3].type.val, 0xFF);
        EXPECT_EQ(track0.getEvents()[3].data.size(), 1);
        EXPECT_EQ(track0.getEvents()[3].data[0], (int)audio::midi::MIDI_META_EVENT::END_OF_TRACK);
        EXPECT_EQ(track0.getEvents()[3].delta_time, 0);
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
        
        EXPECT_EQ(m->getTrack(0).getEvents().size(), 1);
        EXPECT_EQ(m->getTrack(1).getEvents().size(), 29);
        EXPECT_EQ(m->getTrack(2).getEvents().size(), 31);

        // absolute time checks
        auto t = m->getTrack(0);
        EXPECT_EQ(t.getEvents()[0].delta_time, 0);
        
        t = m->getTrack(1);
        EXPECT_EQ(t.getEvents()[0].delta_time, 0);
        EXPECT_EQ(t.getEvents()[1].delta_time, 120);
        EXPECT_EQ(t.getEvents()[2].delta_time, 0);
        EXPECT_EQ(t.getEvents()[3].delta_time, 120);
        // ... 
        EXPECT_EQ(t.getEvents()[28].delta_time, /*1920*/ 0);

        t = m->getTrack(2);
        EXPECT_EQ(t.getEvents()[0].delta_time, 0);
        EXPECT_EQ(t.getEvents()[1].delta_time, 120);
        EXPECT_EQ(t.getEvents()[2].delta_time, 0);
        EXPECT_EQ(t.getEvents()[3].delta_time, 120);
        // ... 
        EXPECT_EQ(t.getEvents()[30].delta_time, /*1920*/ 0);
    }

    void cmp_midievent(const int i, const audio::midi::MIDIEvent& e, const uint32_t exp_delta_time)
    {
        EXPECT_EQ(e.delta_time, exp_delta_time) << "index: " << i;
    }

    bool cmp_midievent(const audio::midi::MIDIEvent& oe1, const audio::midi::MIDIEvent& e2, const uint32_t delta_time)
    {
        const bool dt = oe1.delta_time == e2.delta_time
            || oe1.delta_time - delta_time == e2.delta_time
            || oe1.delta_time == e2.delta_time - delta_time;

        const bool res = dt && oe1.type.val == e2.type.val
            && oe1.data.size() == e2.data.size();
        if (!res)
            return false;

        for (size_t i = 0; i < oe1.data.size(); ++i) {
            if (oe1.data[i] != e2.data[i])
                return false;
        }

        return true;
    }

    TEST(MIDFile, midifile_sample_convert_to_single_track)
    {
        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::MIDI_META_EVENT_TYPES_LOW;
        using audio::midi::MIDI_META_EVENT;
        using audio::midi::MIDIEvent;

        MIDFile f("fixtures/midifile_sample.mid");

        auto m = f.convertToSingleTrackMIDI();
        constexpr size_t expTotalEvents = 1 + 29 + 31 - 3 + 1; // track0,track1,track2, 3 end_of_tracks to 1 end_of_track
        EXPECT_EQ(m->format, audio::midi::MIDI_FORMAT::SINGLE_TRACK);
        EXPECT_EQ(m->numTracks, 1);
        EXPECT_EQ(m->division, 120);
        EXPECT_EQ(m->getTrack(0).getEvents().size(), expTotalEvents);

        int endOfTrackEvents = 0;
        constexpr uint8_t meta_event_val = (static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::META_SYSEX) << 4) | static_cast<uint8_t>(MIDI_META_EVENT_TYPES_LOW::META);
        for (const auto& e : m->getTrack(0).getEvents()) {
            if (e.type.val == meta_event_val && e.data[0] == static_cast<uint8_t>(MIDI_META_EVENT::END_OF_TRACK)) {
                endOfTrackEvents++;
            }
        }

        EXPECT_EQ(endOfTrackEvents, 1);
        
        /*const std::array<uint32_t, 61> exp_abs_times = {
            0, 0, 0, 120, 120, 120, 120, 240, 240, 240, 240, 360, 360, 360,
            360, 480, 480, 480, 480, 600, 600, 600, 600, 720, 720, 720, 720, 840, 840,
            960,  960, 960, 960, 1080, 1080, 1080, 1080, 1200,1200,1200,1200,1320,1320,
            1320,1320,1440,1440,1440,1440,1560,1560,1560,1560,1680,1680,1680,1680,1920,
            1920,1920,1920 };*/

        //const std::array<uint32_t, expTotalEvents> exp_delta_times = {
        //    0,0,0,     // 2
        //    120,0,0,0, // 6
        //    120,0,0,0, // 10
        //    120,0,0,0, // 14
        //    120,0,0,0, // 18
        //    120,0,0,0, // 22
        //    120,0,0,0, // 26
        //    120,0,     // 28
        //    120,0,0,0, // 32
        //    120,0,0,0, // 36
        //    120,0,0,0, // 40
        //    120,0,0,0, // 44
        //    120,0,0,0, // 48
        //    120,0,0,0, // 52
        //    120,0,0,0, // 56
        //    240,0,0,0  // 60
        //};

        //std::vector<std::vector<uint8_t>> v = { { 1,2,3 } };

        // This checks isn't working as it is too strict.
        // it should compare the same absolute time getEvents() data instead.
        // anyway the error is not on reading the file
        //const std::vector<std::vector<uint8_t>> exp_datas = {
        //    {0xff, 0x2f},      {0x90, 0x48, 0x40},{0x90, 0x30, 0x40},                    // 2
        //    {0x80, 0x48, 0x40},{0x90, 0x48, 0x40},{0x80, 0x30, 0x40},{0x90, 0x3c, 0x40}, // 6
        //    {0x80, 0x48, 0x40},{0x90, 0x4f, 0x40},{0x90, 0x40, 0x40},{0x80, 0x3c, 0x40}, // 10
        //    {0x80, 0x4f, 0x40},{0x90, 0x4f, 0x40},{0x90, 0x3c, 0x40},{0x80, 0x40, 0x40}, // 14
        //    {0x80, 0x4f, 0x40},{0x90, 0x51, 0x40},{0x90, 0x41, 0x40},{0x80, 0x3c, 0x40}, // 18
        //    {0x80, 0x51, 0x40},{0x90, 0x51, 0x40},{0x90, 0x3c, 0x40},{0x80, 0x41, 0x40}, // 22
        //    {0x80, 0x51, 0x40},{0x90, 0x4f, 0x40},{0x90, 0x40, 0x40},{0x80, 0x3c, 0x40}, // 26
        //    {0x80, 0x40, 0x40},{0x90, 0x3c, 0x40},                                       // 28
        //    {0x80, 0x4f, 0x40},{0x90, 0x4d, 0x40},{0x90, 0x3e, 0x40},{0x80, 0x3c, 0x40}, // 32
        //    {0x80, 0x4d, 0x40},{0x90, 0x4d, 0x40},{0x80, 0x3e, 0x40},{0x90, 0x3b, 0x40}, // 36
        //    {0x80, 0x4d, 0x40},{0x90, 0x4c, 0x40},{0x80, 0x3b, 0x40},{0x90, 0x3c, 0x40}, // 40
        //    {0x80, 0x4c, 0x40},{0x90, 0x4c, 0x40},{0x80, 0x3c, 0x40},{0x90, 0x39, 0x40}, // 44
        //    {0x80, 0x4c, 0x40},{0x90, 0x4a, 0x40},{0x80, 0x39, 0x40},{0x90, 0x35, 0x40}, // 48
        //    {0x80, 0x4a, 0x40},{0x90, 0x4a, 0x40},{0x80, 0x35, 0x40},{0x90, 0x37, 0x40}, // 52
        //    {0x90, 0x48, 0x40},{0x80, 0x4a, 0x40},{0x80, 0x37, 0x40},{0x90, 0x30, 0x40}, // 56
        //    {0x80, 0x48, 0x40},{0xff, 0x2f},      {0x80, 0x30, 0x40},{0xff, 0x2f},       // 60
        //};

        /*auto t = m->getTrack(0);

        for (int i = 0; i < 61; i++) {
            cmp_midievent(i, t.getEvents()[i], exp_delta_times[i]);
        }*/

        auto origMidi = f.getMIDI();
        auto midiEvents = m->getTrack().getEvents();
        size_t totalMeChecked = 0;
        
        for (int n = 0; n < origMidi->numTracks; n++) {
            auto origTrack = origMidi->getTrack(n);
            auto origTrackEvents = origTrack.getEvents();
            const size_t totTrackEvent = origTrackEvents.size();
            std::vector<MIDIEvent> meOrig;
            uint32_t delta_time = 0;
            for (int i = 0; i < totTrackEvent; i++) {
                auto origEvent = origTrackEvents.at(i);
                if (origEvent.type.val == meta_event_val
                    && origEvent.data[0] == static_cast<uint8_t>(MIDI_META_EVENT::END_OF_TRACK)) {
                    continue; // need to check only the last one, after the loop
                }

                if (origEvent.delta_time != 0) {
                    // here do the check of the same delta time block for this track.
                    auto it = midiEvents.begin();

                    while (meOrig.size() > 0)
                    {
                        bool res = false;
                        auto midiEvent = *it;

                        for (auto itOrig = meOrig.begin(); itOrig != meOrig.end();) {
                            if(cmp_midievent(*itOrig, midiEvent, delta_time))
                            {
                                // at least 1 valid
                                itOrig = meOrig.erase(itOrig);
                                if (it->delta_time == 0 || n == origMidi->numTracks - 1)
                                {
                                    midiEvents.erase(it);
                                    res = true;
                                }

                                ++totalMeChecked;
                                break;
                            }
                            else
                                ++itOrig;
                        }

                        if (res)
                            it = midiEvents.begin();
                        else {
                           // do {
                                ++it;
                          //  } while (it != midiEvents.end() && it->delta_time != 0 && it->delta_time != delta_time);
                        }
                        if (it == midiEvents.end()) {
                            break;
                        }
                    }

                    EXPECT_EQ(meOrig.size(), 0) << "n: " << n << " i: " << i;
                    for(const auto& me:meOrig) {
                        // debug
                        std::cout << "dt: " << me.delta_time << " type: " << (int)me.type.val << " data: ";
                        for (const auto& d : me.data) {
                            std::cout << (int)d << " ";
                        }

                        std::cout << std::endl;
                    }
                    // clear to be ready for the next one
                    meOrig.clear();
                    delta_time = origEvent.delta_time;
                }

                meOrig.emplace_back(origEvent);


                // expect to be almost the same sequence at the same abs_time, but delta_time won't

                // TODO: check from the converted the same group of delta_time events
                //       if are the same ?? contained?
                //       as if 2 events to be played at the same time one is after the another is ok.

                //EXPECT_EQ(midiEvent.type.val, origEvent.type.val) << "index i: " << i << " k: " << k;
                //EXPECT_EQ(midiEvent.delta_time, origEvent.delta_time) << "index i: " << i << " k: " << k;
                //EXPECT_EQ(midiEvent.data.size(), origEvent.data.size()) << "index i: " << i << " k: " << k;
                //for (int j = 0; j < midiEvent.data.size(); j++) {
                //   EXPECT_EQ(midiEvent.data.at(j), origEvent.data.at(j)) << "index i: " << i << " k: " << k << " j: " << j;
                //}
            }
        }

        EXPECT_EQ(totalMeChecked, expTotalEvents);
        EXPECT_EQ(midiEvents.size(), 0);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
