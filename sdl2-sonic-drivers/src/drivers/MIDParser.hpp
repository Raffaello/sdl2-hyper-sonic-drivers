#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/MIDI.hpp>
#include <audio/midi/types.hpp>
#include <memory>
#include <cstdint>

namespace drivers
{
    // Test class/experimental to understand midi events
    class MIDParser
    {
    public:
        MIDParser(std::shared_ptr<audio::MIDI> midi, std::shared_ptr<audio::scummvm::Mixer> mixer);
        virtual ~MIDParser();

        void processTrack(const audio::midi::MIDITrack& track, const int i);
        //void incTicks();
        void display();
    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        std::shared_ptr<audio::MIDI> _midi;

        int num_tracks;
        int division;
        uint32_t tempo;
    };
}
