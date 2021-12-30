#pragma once

#include <audio/MIDI.hpp>
#include <audio/midi/types.hpp>
#include <memory>
#include <cstdint>

#include <RtMidi.h> // todo remove

namespace drivers
{
    // Test class/experimental to understand midi events
    class MIDParser
    {
    public:
        explicit MIDParser(std::shared_ptr<audio::MIDI> midi);
        virtual ~MIDParser();

        void processTrack(audio::midi::MIDITrack& track, const int i, std::shared_ptr<RtMidiOut> midiout);
        //void incTicks();
        void display(std::shared_ptr<RtMidiOut> midiout);
    private:
        std::shared_ptr<audio::MIDI> _midi;

        int32_t tempo;
    };
}
