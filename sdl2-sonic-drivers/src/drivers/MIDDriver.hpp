#pragma once

#include <audio/MIDI.hpp>
#include <audio/midi/types.hpp>
#include <memory>
#include <cstdint>
#include <drivers/midi/Device.hpp>

namespace drivers
{
    class MIDDriver
    {
    public:
        MIDDriver() = default;
        ~MIDDriver() = default;
        // TODO for now just OPL, later on all others
        // TODO need to be async
        bool playMidi(const std::shared_ptr<audio::MIDI> midi, midi::Device& device);
    private:
        void processTrack(const audio::midi::MIDITrack& track, midi::Device& device, const uint16_t division);
        bool isPlaying = false;
        uint32_t _tempo;
    };
}
