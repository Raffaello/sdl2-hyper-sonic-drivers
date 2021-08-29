#pragma once

#include <cstdint>

namespace hardware
{
    /// <summary>
    /// @see https://moddingwiki.shikadi.net/wiki/OPL_chip
    /// </summary>
    namespace opl
    {
        namespace woody
        {
            enum class ChipType
            {
                OPL2,
                OPL2_DUAL,
                OPL3
            };

            //this opl class could be moved just in hardware namespace as a general interface for opl namespace
            class OPL
            {
            public:
                OPL(const ChipType chip) noexcept;
                ChipType getChipType() const noexcept;

                virtual void update(int16_t* buf, const int32_t samples) = 0;
                virtual void write(const int reg, const int val) = 0; // combined register select + data write
                //virtual void setchip(int n) // select OPL chip
                //virtual void init() = 0; // reinitialize OPL chip(s)
                virtual int32_t getSampleRate() const noexcept = 0;
            protected:
                ChipType _chip;
            };
        }
    }
}
