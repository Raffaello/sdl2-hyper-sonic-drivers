#pragma once
#include <hardware/opl/dosbox/Timer.hpp>
#include <cstdint>

namespace hardware::opl::dosbox
{
    struct Chip
    {
        Chip() = default;
        ~Chip() = default;
        //Last selected register
        Timer timer[2];
        //Check for it being a write to the timer
        bool write(uint32_t addr, uint8_t val);
        //Read the current timer state, will use current double
        uint8_t read();
    };
}
