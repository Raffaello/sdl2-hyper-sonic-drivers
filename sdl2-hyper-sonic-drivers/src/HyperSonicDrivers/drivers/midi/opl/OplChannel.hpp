#pragma once

#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    class OplChannel : public IMidiChannel
    {
    public:
        explicit OplChannel(const uint8_t channel);
    };
}
