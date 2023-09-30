#pragma once

#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>

namespace HyperSonicDrivers::drivers::midi
{
    /**
    * Interface for Midi Channel Voice message
    **/
    class IMidiChannelVoice
    {
    public:
        IMidiChannelVoice() = default;
        virtual ~IMidiChannelVoice() = default;

    protected:
        IMidiChannel* m_channel = nullptr;  // MIDI channel
        uint8_t m_note = 0;                 /* note number */
        int16_t m_pitch_factor = 0;         /* pitch-wheel value */
        bool m_free = true;
        bool m_sustain = false;
        bool m_vibrato = false;
    };

}
