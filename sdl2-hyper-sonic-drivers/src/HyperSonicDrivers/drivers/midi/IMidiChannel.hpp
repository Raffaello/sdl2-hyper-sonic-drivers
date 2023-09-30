#pragma once

#include <cstdint>
#include <list>

namespace HyperSonicDrivers::drivers::midi
{
    /**
    * Interface for MIDI operation to a specific MIDI Channel
    **/
    class IMidiChannel
    {
    public:
        explicit IMidiChannel(const uint8_t channel);
        virtual ~IMidiChannel() = default;

        const uint8_t channel;         // MIDI channel, not used
        uint8_t volume = 0;            // channel volume
        uint8_t pan = 0;               // pan, 0=normal
        uint8_t pitch = 0;             // pitch wheel, 0=normal
        uint8_t sustain = 0;           // sustain pedal value
        uint8_t modulation = 0;        // modulation pot value
        uint8_t program = 0;           // instrument number
        const bool isPercussion;

    protected:

        // MIDI Events
        //virtual void noteOff(const uint8_t note) noexcept = 0;
        //virtual void noteOn(const uint8_t note, const uint8_t vol) noexcept = 0;
        //virtual void controller(const uint8_t ctrl, uint8_t value) noexcept = 0;
        //virtual void programChange(const uint8_t program) noexcept = 0;
        //virtual void pitchBend(const uint16_t bend) noexcept = 0;

        // MIDI Controller Events
        //virtual void ctrl_modulationWheel(const uint8_t value) const noexcept = 0;
        //virtual void ctrl_volume(const uint8_t value) const noexcept = 0;
        //virtual void ctrl_panPosition(uint8_t value) const noexcept = 0;
        //virtual void ctrl_sustain(uint8_t value) const noexcept = 0;
    };
}
