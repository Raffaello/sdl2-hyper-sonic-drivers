#include <HyperSonicDrivers/drivers/midi/opl/OplChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>
#include <memory>

namespace HyperSonicDrivers::drivers::midi::opl
{
    OplChannel::OplChannel(const uint8_t channel) :
        IMidiChannel(channel)
    {
    }
}
