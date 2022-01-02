#pragma once

#include <audio/scummvm/Mixer.hpp>
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
        explicit MIDDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<midi::Device> device);
        ~MIDDriver() = default;
        // TODO need to be async
        void play(const std::shared_ptr<audio::MIDI> midi) const noexcept;
        void stop();
        void pause();
        bool isPlaying();
    private:
        void processTrack(const audio::midi::MIDITrack& track, const uint16_t division) const noexcept;

        // mixer is not used, but ensuring is initialized
        // if not initialized there are delays otherwise
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        std::shared_ptr<midi::Device> _device;
    };
}
