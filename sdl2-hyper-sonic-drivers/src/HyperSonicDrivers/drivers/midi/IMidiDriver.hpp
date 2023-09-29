#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>

namespace HyperSonicDrivers::drivers::midi
{
    /**
    * Interface for all the different Midi drivers
    * so it can be easily used from MIDDrv (maybe rename it to MIDI_player or something)
    * TODO/NOTE: it might be "merged" with IMusicDriver ...
    **/
    class IMidiDriver
    {
    public:
        IMidiDriver() = default;
        virtual ~IMidiDriver() = default;

        virtual bool open(const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) = 0;
        virtual void close() = 0;

        inline bool isOpen() const noexcept { return m_isOpen; }

        virtual void send(const audio::midi::MIDIEvent& e) /*const*/ noexcept = 0;
        virtual void send(uint32_t msg) = 0;
        virtual void send(int8_t channel, uint32_t msg) = 0;

        virtual void pause() const noexcept = 0;
        virtual void resume() const noexcept = 0;

    protected:
        bool m_isOpen = false;

        virtual void onCallback() noexcept = 0;

        // MIDI events
        //virtual void noteOff(const uint8_t chan, const uint8_t note) noexcept = 0;
        //virtual void noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol) noexcept = 0;
        //virtual void controller(const uint8_t chan, const uint8_t ctrl, uint8_t value) noexcept = 0;
        //virtual void programChange(const uint8_t chan, const uint8_t program) noexcept = 0;
        //virtual void pitchBend(const uint8_t chan, const uint16_t bend) noexcept = 0;

        // MIDI Controller Events
        //virtual void ctrl_modulationWheel(const uint8_t chan, const uint8_t value) const noexcept = 0;
        //virtual void ctrl_volume(const uint8_t chan, const uint8_t value) const noexcept = 0;
        //virtual void ctrl_panPosition(const uint8_t chan, uint8_t value) const noexcept = 0;
        //virtual void ctrl_sustain(const uint8_t chan, uint8_t value) const noexcept = 0;


    };
}
