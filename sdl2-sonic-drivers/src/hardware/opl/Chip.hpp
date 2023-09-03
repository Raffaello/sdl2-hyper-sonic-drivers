#pragma once
#include <hardware/opl/Timer.hpp>
#include <cstdint>
#include <array>

namespace HyperSonicDrivers::hardware::opl
{
    class Chip
    {
    public:
        typedef union register_u
        {
            uint16_t normal;
            uint8_t dual[2];
        } register_u;

        Chip() = default;
        ~Chip() = default;

        //Check for it being a write to the timer
        bool write(const uint32_t addr, const uint8_t val) noexcept;
        
        //Read the current timer state, will use current double
        uint8_t read() noexcept;

    protected:
        //Last selected register
        std::array<Timer, 2> timer;
    };
}
