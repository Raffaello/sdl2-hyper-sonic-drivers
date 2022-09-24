#pragma once

#include <cstdint>

namespace hardware
{
    /// <summary>
    /// @see https://moddingwiki.shikadi.net/wiki/OPL_chip
    /// </summary>
    namespace opl::woody
    {
        enum class ChipType
        {
            OPL2,
            OPL2_DUAL,
            //OPL3
        };

        // TODO flat this hierachy to the other EmulatedOPL instead
        class OPL
        {
        public:
            OPL(const ChipType chip) noexcept;
            ChipType getChipType() const noexcept;
            virtual ~OPL() = default;

            virtual void update(int16_t* buf, const int32_t samples) = 0;
            virtual void write(const uint32_t reg, const uint8_t val) = 0; // combined register select + data write
            virtual void init() = 0;
            virtual int32_t getSampleRate() const noexcept = 0;
            virtual bool isStereo() const = 0;
        protected:
            ChipType _chip;
        };
    }
}
