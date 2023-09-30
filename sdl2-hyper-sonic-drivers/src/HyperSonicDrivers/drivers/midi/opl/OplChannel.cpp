#include <HyperSonicDrivers/drivers/midi/opl/OplChannel.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    OplChannel::OplChannel(const uint8_t channel)
        : IMidiChannel(channel)
    {
    }
}
