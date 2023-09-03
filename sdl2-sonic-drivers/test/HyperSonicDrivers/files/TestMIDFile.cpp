#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/files/MIDFile.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/audio/MIDI.hpp>
#include <HyperSonicDrivers/audio/midi/MIDITrack.hpp>
#include <array>
#include <cstdint>

namespace HyperSonicDrivers::files
{
    using audio::midi::MIDIEvent;

    TEST(MIDFile, cstorDefault)
    {
        MIDFile f("fixtures/MI_intro.mid");
        
        EXPECT_EQ(f.getOriginalMIDI()->format, audio::midi::MIDI_FORMAT::SIMULTANEOUS_TRACK);
        EXPECT_EQ(f.getOriginalMIDI()->numTracks, 15);
        EXPECT_EQ(f.getOriginalMIDI()->division, 192);

        auto track0 = f.getOriginalMIDI()->getTrack(0);
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

        auto m = f.getOriginalMIDI();
        
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

    bool cmp_midievent(const audio::midi::MIDIEvent& oe1, const audio::midi::MIDIEvent& e2)
    {
        // original event delta time can be greater, due to another track playing
        // an event in between.
        // so it is almost pointless check the original delta time with the single track..
        
        //const bool dt = oe1.delta_time == e2.delta_time
        //    || oe1.delta_time - delta_time == e2.delta_time
        //    || oe1.delta_time == e2.delta_time - delta_time;

        const bool res = /*dt && */oe1.type.val == e2.type.val
            && oe1.data.size() == e2.data.size();
        if (!res)
            return false;

        for (size_t i = 0; i < oe1.data.size(); ++i) {
            if (oe1.data[i] != e2.data[i])
                return false;
        }

        return true;
    }

    int cmp_midiEvents(std::vector<MIDIEvent>& meOrig, std::vector<MIDIEvent>& midiEvents)
    {
        size_t totalMeChecked = 0;
        auto it = midiEvents.begin();
        while (meOrig.size() > 0)
        {
            bool res = false;
            auto midiEvent = *it;

            for (auto itOrig = meOrig.begin(); itOrig != meOrig.end();) {
                if (cmp_midievent(*itOrig, midiEvent))
                {
                    // at least 1 valid
                    itOrig = meOrig.erase(itOrig);
                    midiEvents.erase(it);
                    res = true;
                    ++totalMeChecked;
                    break;
                }
                else
                    ++itOrig;
            }

            if (res)
                it = midiEvents.begin();
            else {
                ++it;
            }
            if (it == midiEvents.end()) {
                break;
            }
        }

        EXPECT_EQ(meOrig.size(), 0); //<< "n: " << n << " i: " << i;
        // DEBUG
        //for (const auto& me : meOrig) {
        //    // debug
        //    std::cout << "dt: " << me.delta_time << " type: " << (int)me.type.val << " data: ";
        //    for (const auto& d : me.data) {
        //        std::cout << (int)d << " ";
        //    }

        //    std::cout << std::endl;
        //}

        return totalMeChecked;
    }

    TEST(MIDFile, midifile_sample_convert_to_single_track)
    {
        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::MIDI_META_EVENT_TYPES_LOW;
        using audio::midi::MIDI_META_EVENT;

        MIDFile f("fixtures/midifile_sample.mid");

        auto m = f.getMIDI();
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

        auto origMidi = f.getOriginalMIDI();
        auto midiEvents = m->getTrack().getEvents();
        size_t totalMeChecked = 0;
        size_t notErased = 0;
        for (int n = 0; n < origMidi->numTracks; n++) {
            auto origTrack = origMidi->getTrack(n);
            auto origTrackEvents = origTrack.getEvents();
            const size_t totTrackEvent = origTrackEvents.size();
            std::vector<MIDIEvent> meOrig;
            for (int i = 0; i < totTrackEvent; i++) {
                auto origEvent = origTrackEvents.at(i);
                if (origEvent.type.val == meta_event_val
                    && origEvent.data[0] == static_cast<uint8_t>(MIDI_META_EVENT::END_OF_TRACK)) {
                    continue; // need to check only the last one, after the loop
                }

                if (origEvent.delta_time != 0) {
                    // here do the check of the same delta time block for this track.
                    totalMeChecked += cmp_midiEvents(meOrig, midiEvents);
                    meOrig.clear();
                }

                meOrig.emplace_back(origEvent);
            }

            if (meOrig.size() > 0) {
                totalMeChecked += cmp_midiEvents(meOrig, midiEvents);
            }
        }

        EXPECT_EQ(notErased, 0);
        EXPECT_EQ(totalMeChecked, expTotalEvents - 1);
        EXPECT_EQ(midiEvents.size(), 1);
        // last element is the end of track
        EXPECT_EQ(midiEvents.at(0).type.val, meta_event_val);
        EXPECT_EQ(midiEvents.at(0).data.at(0), static_cast<uint8_t>(MIDI_META_EVENT::END_OF_TRACK));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
