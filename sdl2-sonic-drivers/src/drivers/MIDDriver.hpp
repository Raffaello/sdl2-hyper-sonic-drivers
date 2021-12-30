#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/MIDI.hpp>
#include <audio/midi/types.hpp>
#include <memory>
#include <cstdint>

namespace drivers
{
    class MIDDriver
    {
    public:
        MIDDriver(std::shared_ptr<audio::scummvm::Mixer> mixer);
        ~MIDDriver() = default;
        
        bool playMidi(const std::shared_ptr<audio::MIDI> midi);
    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        //std::shared_ptr<audio::MIDI> _midi;
    };
}
