#pragma once

#include <memory>
#include <cstdint>
#include <thread>
#include <atomic>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/audio/MIDI.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/drivers/IMusicDriver.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>


namespace HyperSonicDrivers::drivers
{
    /**
    * Not sure what is this class, a wrapper around different midi driver
    * TODO: remove the thread and perform through callbacks
    * @deprecated class start over with a new class MidiPlayer
    **/
    class MIDDriver : public IMusicDriver
    {
    public:
        explicit MIDDriver(
            const std::shared_ptr<devices::IDevice>& device,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume = 255,
            const uint8_t pan = 0
        );
        ~MIDDriver() override;

        void setMidi(const std::shared_ptr<audio::MIDI>& midi) noexcept;
        // It works only for Opl devices
        bool loadBankOP2(const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank) noexcept;
        // this restore the default MidiDriver (scummvm::MidiAdlib, MT32)
        bool resetBankOP2() noexcept;

        void play(const uint8_t track = 0) noexcept override;
        void stop() noexcept override;

        void pause() noexcept;
        void resume() noexcept;

        bool isPlaying() const noexcept override;

        bool isPaused() const noexcept;
        inline bool isTempoChanged() const noexcept { return m_midiTempoChanged; }
        inline uint32_t getTempo() noexcept { m_midiTempoChanged = false; return m_tempo; }
    protected:
        void processTrack(const audio::midi::MIDITrack& track, const uint16_t division);
        // TODO: can be later on moved to public, but not sure the tempoChanged event
        //       would be better using SDL2 custom event 
        //       or a event queue sub-sytem instead of doing this with a simple boolean
        inline void setTempo(const uint32_t tempo) noexcept { m_midiTempoChanged = true; m_tempo = tempo; }
        bool open_() noexcept;
    private:
        std::shared_ptr<devices::IDevice> m_device;
        std::unique_ptr<drivers::midi::IMidiDriver> m_midiDriver;
        std::shared_ptr<audio::MIDI> m_midi;
        const audio::mixer::eChannelGroup m_group;
        const uint8_t m_volume;
        const uint8_t m_pan;
        
        std::jthread m_player;

        std::atomic<bool> m_isPlaying = false;
        std::atomic<bool> m_force_stop = false;
        std::atomic<bool> m_paused = false;
        std::atomic<bool> m_midiTempoChanged = false;
        std::atomic<uint32_t> m_tempo = 0;
    };
}
