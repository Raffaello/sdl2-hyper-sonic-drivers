#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>

namespace HyperSonicDrivers::drivers::midi
{
    IMidiChannel::IMidiChannel(const uint8_t channel) :
        channel(channel),
        isPercussion(channel == audio::midi::MIDI_PERCUSSION_CHANNEL)
    {}
}
