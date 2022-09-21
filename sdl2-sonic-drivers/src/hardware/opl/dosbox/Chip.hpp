#pragma once
#include <hardware/opl/dosbox/Timer.hpp>
#include <cstdint>

namespace hardware::opl::dosbox
{
    class Chip
    {
    public:
        Chip() = default;
        ~Chip() = default;

        //Check for it being a write to the timer
        bool write(const uint32_t addr, const uint8_t val) noexcept;
        
        //Read the current timer state, will use current double
        uint8_t read() noexcept;

    protected:
        //Last selected register
        Timer timer[2];
    };
}
