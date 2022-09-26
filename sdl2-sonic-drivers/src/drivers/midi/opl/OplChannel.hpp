#pragma once

#include <cstdint>
#include <memory>
#include <audio/opl/banks/OP2Bank.hpp>

namespace drivers::midi::opl
{
    /// <summary>
    /// More than one note can be played at once in one channel.
    /// It means more than one OplVoice can be associated to OplChannel.
    /// </summary>
    class OplChannel
    {
    public:
        OplChannel() = delete;
        explicit OplChannel(const uint8_t channel_);
        ~OplChannel() = default;

        const uint8_t channel;         // MIDI channel, not used
        uint8_t volume = 0;            // volume
        uint8_t pan = 0;               // pan, 0=normal
        uint8_t pitch = 0;             // pitch wheel, 0=normal
        uint8_t sustain = 0;           // sustain pedal value
        uint8_t modulation = 0;        // modulation pot value

        // Regular messages
        void programChange(const uint8_t program);
        inline uint8_t getProgram() const noexcept { return _program; }
    private:
        const bool _isPercussion;
        uint8_t _program = 0;          // instrument number
    };
}
