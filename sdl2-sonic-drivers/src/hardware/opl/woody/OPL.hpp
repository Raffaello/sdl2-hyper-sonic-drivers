#pragma once

#include <cstdint>

namespace hardware
{
    /// <summary>
    /// @see https://moddingwiki.shikadi.net/wiki/OPL_chip
    /// </summary>
    namespace opl::woody
    {
        /**
        * Woody OPL Interface
        */
        class OPL
        {
        public:
            OPL(const OPL&) = delete;
            OPL(const OPL&&) = delete;
            OPL& operator=(const  OPL&) = delete;

            OPL() = default;
            virtual ~OPL() = default;

            virtual void update(int16_t* buf, const int32_t samples) = 0;
            virtual void write(const uint32_t reg, const uint8_t val) = 0; // combined register select + data write
            virtual uint8_t read(const uint32_t port) noexcept = 0;
            virtual void writeReg(const uint16_t r, const uint16_t v) noexcept = 0;
            virtual void init() = 0;
        };
    }
}
