#pragma once

#include <memory>
#include <cstdint>
#include <thread>
#include <atomic>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/audio/MIDI.hpp>
#include <HyperSonicDrivers/drivers/midi/Device.hpp>

namespace HyperSonicDrivers::drivers
{
    class MIDDriver
    {
    public:
        explicit MIDDriver(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const std::shared_ptr<midi::Device>& device);
        ~MIDDriver();
        void play(const std::shared_ptr<audio::MIDI>& midi) noexcept;
        void stop() noexcept;
        void pause() noexcept;
        void resume() noexcept;
        bool isPlaying() const noexcept;
        bool isPaused() const noexcept;
        inline bool isTempoChanged() const noexcept { return _midiTempoChanged; }
        inline uint32_t getTempo() noexcept { _midiTempoChanged = false; return _tempo; }
    protected:
        void processTrack(const audio::midi::MIDITrack& track, const uint16_t division);
        // TODO: can be later on moved to public, but not sure the tempoChanged event
        //       would be better using SDL2 custom event 
        //       or a event queue sub-sytem instead of doing this with a simple boolean
        inline void setTempo(const uint32_t tempo) noexcept { _midiTempoChanged = true; _tempo = tempo; }

    private:
        // mixer is not used, but ensuring is initialized
        // if not initialized there are delays otherwise
        std::shared_ptr<audio::scummvm::Mixer> _mixer; // TODO can be removed
        std::shared_ptr<midi::Device> _device;

        // TODO: consider to create a utils/Thread class
        //       to handle for each OS specific realtime and initialization step.
        std::thread _player;

        std::atomic<bool> _isPlaying = false;
        std::atomic<bool> _force_stop = false;
        std::atomic<bool> _paused = false;
        std::atomic<bool> _midiTempoChanged = false;
        std::atomic<uint32_t> _tempo = 0;
    };
}
