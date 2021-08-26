#pragma once

#include <cstdint>

namespace hardware
{
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

            //TODO: remove and replace SurrondOPL and WoodyEmuOPL to extend from EmulatedOPL
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
                // TODO: review this variable as it is almost useless?
                //int8_t   _currentChip;       // currently selected OPL chip number (0 or 1 actually)
            };
        }
    }
}
