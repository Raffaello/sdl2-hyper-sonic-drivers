#include <audio/midi/types.hpp>
#include <drivers/midi/opl/OplChannel.hpp>
#include <format>
#include <SDL2/SDL_log.h>

namespace drivers::midi::opl
{
    using audio::midi::MIDI_PERCUSSION_CHANNEL;

    OplChannel::OplChannel(const uint8_t channel_) :
        channel(channel_), _isPercussion(channel_ == MIDI_PERCUSSION_CHANNEL)
    {
    }

    void OplChannel::programChange(const uint8_t program)
    {
        if (program > 127) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, std::format("Progam change value >= 127 -> {}", program).c_str());
        }

        // NOTE: if program is not changed shouldn't be required to do anything ...
        _program = program;
        //spdlog::debug("program change {} {} ({})", channel, program, _isPercussion);
    }
}
