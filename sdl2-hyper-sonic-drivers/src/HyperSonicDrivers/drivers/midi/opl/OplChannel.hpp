#pragma once

#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    class OplChannel : public IMidiChannel
    {
    public:
        OplChannel() = default;
    };
}
