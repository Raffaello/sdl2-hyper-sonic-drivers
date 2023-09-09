#include <format>
#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplChannel.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    using audio::midi::MIDI_PERCUSSION_CHANNEL;
    using utils::logW;

    OplChannel::OplChannel(const uint8_t channel_) :
        channel(channel_), _isPercussion(channel_ == MIDI_PERCUSSION_CHANNEL)
    {
    }

    void OplChannel::programChange(const uint8_t program)
    {
        if (program > 127)
        {
            logW(std::format("Progam change value >= 127 -> {}", program), this);
        }

        // NOTE: if program is not changed shouldn't be required to do anything ...
        _program = program;
    }
}
