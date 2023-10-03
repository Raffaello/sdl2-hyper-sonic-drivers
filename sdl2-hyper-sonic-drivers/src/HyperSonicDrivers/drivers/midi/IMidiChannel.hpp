#pragma once

#include <cstdint>
//#include <list>
#include <HyperSonicDrivers/drivers/midi/IMidiChannelVoice.hpp>

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

        const uint8_t channel;         // MIDI channel number
        uint8_t volume = 0;            // channel volume
        uint8_t pan = 64;               // pan, 64=center
        uint16_t pitch = 0;            // pitch wheel, 0=normal
        uint8_t sustain = 0;           // sustain pedal value
        uint8_t modulation = 0;        // modulation pot value
        uint8_t program = 0;           // instrument number
        const bool isPercussion;
    };
}
