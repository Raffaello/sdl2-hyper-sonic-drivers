#pragma once

#include <memory>
#include <HyperSonicDrivers/hardware/opl/EmulatedOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/woody/OPL.hpp>

namespace HyperSonicDrivers::hardware
{
    namespace opl
    {
        namespace woody
        {
            class WoodyOPL : public EmulatedOPL
            {
            public:
                WoodyOPL(const std::shared_ptr<audio::IMixer>& mixer, const bool surround);
                ~WoodyOPL() override = default;

                bool init() override;
                void reset() override;
                void write(const uint32_t port, const uint16_t val) noexcept override;
                uint8_t read(const uint32_t port) noexcept override;
                void writeReg(const uint16_t r, const uint16_t v) noexcept override;

            protected:
                void generateSamples(int16_t* buffer, const size_t length) noexcept override;

            private:
                std::unique_ptr<woody::OPL> _opl = nullptr;
            };
        }
    }
}
