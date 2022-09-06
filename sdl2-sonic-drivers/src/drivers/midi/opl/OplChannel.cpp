#include <audio/midi/types.hpp>
#include <drivers/midi/opl/OplChannel.hpp>
#include <spdlog/spdlog.h>

namespace drivers
{
    namespace midi
    {
        namespace opl
        {
            using audio::midi::MIDI_PERCUSSION_CHANNEL;

            OplChannel::OplChannel(const uint8_t channel_) :
                channel(channel_), _isPercussion(channel_ == MIDI_PERCUSSION_CHANNEL)
            {
            }

            void OplChannel::programChange(const uint8_t program)
            {
                if (program > 127) {
                    spdlog::warn("Progam change value >= 127 -> {}", program);
                }

                // NOTE: if program is not changed shouldn't be required to do anything ...
                _program = program;
                //spdlog::debug("program change {} {} ({})", channel, program, _isPercussion);
            }
        }
    }
}
