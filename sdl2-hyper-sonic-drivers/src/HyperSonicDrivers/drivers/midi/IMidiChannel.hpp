#pragma once

#include <cstdint>

namespace HyperSonicDrivers::drivers::midi
{
    /**
    * Interface for MIDI operation to a specific MIDI Channel
    **/
    class IMidiChannel
    {
    public:
        IMidiChannel() = default;
        virtual ~IMidiChannel() = default;

        // MIDI Events
        virtual void noteOff(const uint8_t note) noexcept = 0;
        virtual void noteOn(const uint8_t note, const uint8_t vol) noexcept = 0;
        virtual void controller(const uint8_t ctrl, uint8_t value) noexcept = 0;
        virtual void programChange(const uint8_t program) noexcept = 0;
        virtual void pitchBend(const uint16_t bend) noexcept = 0;

        // MIDI Controller Events
        virtual void ctrl_modulationWheel(const uint8_t value) const noexcept = 0;
        virtual void ctrl_volume(const uint8_t value) const noexcept = 0;
        virtual void ctrl_panPosition(uint8_t value) const noexcept = 0;
        virtual void ctrl_sustain(uint8_t value) const noexcept = 0;
    };
}
