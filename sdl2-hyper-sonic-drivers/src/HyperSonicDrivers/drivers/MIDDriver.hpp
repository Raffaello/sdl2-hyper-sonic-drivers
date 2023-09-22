#pragma once

#include <memory>
#include <cstdint>
#include <thread>
#include <atomic>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/audio/MIDI.hpp>
#include <HyperSonicDrivers/devices/IMidiDevice.hpp>

namespace HyperSonicDrivers::drivers
{
    // TODO/NOTE: this is more the OnTimer logic and setup of OplDriver, but generalized
    //            for whatever midi::driver ... need to be reivewed
    // TODO: maybe rename it to midi_player
    class MIDDriver
    {
    public:
        explicit MIDDriver(const std::shared_ptr<devices::IMidiDevice>& device);
        ~MIDDriver();
        void play(const std::shared_ptr<audio::MIDI>& midi) noexcept;
        void stop() noexcept;
        void pause() noexcept;
        void resume() noexcept;
        bool isPlaying() const noexcept;
        bool isPaused() const noexcept;
        inline bool isTempoChanged() const noexcept { return m_midiTempoChanged; }
        inline uint32_t getTempo() noexcept { m_midiTempoChanged = false; return m_tempo; }
    protected:
        void processTrack(const audio::midi::MIDITrack& track, const uint16_t division);
        // TODO: can be later on moved to public, but not sure the tempoChanged event
        //       would be better using SDL2 custom event 
        //       or a event queue sub-sytem instead of doing this with a simple boolean
        inline void setTempo(const uint32_t tempo) noexcept { m_midiTempoChanged = true; m_tempo = tempo; }

    private:
        std::shared_ptr<devices::IMidiDevice> m_device;

        // TODO: consider to create a utils/Thread class
        //       to handle for each OS specific realtime and initialization step.
        std::jthread m_player;

        std::atomic<bool> m_isPlaying = false;
        std::atomic<bool> m_force_stop = false;
        std::atomic<bool> m_paused = false;
        std::atomic<bool> m_midiTempoChanged = false;
        std::atomic<uint32_t> m_tempo = 0;
    };
}
