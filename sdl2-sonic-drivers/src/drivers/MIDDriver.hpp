#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/midi/types.hpp>
#include <audio/MIDI.hpp>
#include <drivers/midi/Device.hpp>
#include <memory>
#include <cstdint>
#include <thread>
#include <atomic>

namespace drivers
{
    class MIDDriver
    {
    public:
        explicit MIDDriver(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const std::shared_ptr<midi::Device>& device);
        ~MIDDriver();
        void play(const std::shared_ptr<audio::MIDI>& midi) noexcept;
        void stop(/*const bool wait = false*/) noexcept;
        void pause() noexcept;
        void resume() noexcept;
        bool isPlaying() const noexcept;
        bool isPaused() const noexcept;
    protected:
        void processTrack(const audio::midi::MIDITrack& track, const uint16_t division);

    private:
        // mixer is not used, but ensuring is initialized
        // if not initialized there are delays otherwise
        std::shared_ptr<audio::scummvm::Mixer> _mixer; // TODO can be removed
        std::shared_ptr<midi::Device> _device;

        std::thread _player;
        std::atomic<bool> _isPlaying = false;
        std::atomic<bool> _force_stop = false;
        std::atomic<bool> _paused = false;
    };
}
