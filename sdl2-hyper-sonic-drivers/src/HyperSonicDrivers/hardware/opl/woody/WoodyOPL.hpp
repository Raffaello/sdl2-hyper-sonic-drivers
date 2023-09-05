#pragma once

#include <memory>
#include <HyperSonicDrivers/hardware/opl/scummvm/EmulatedOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/woody/OPL.hpp>

namespace HyperSonicDrivers::hardware
{
    namespace opl
    {
        namespace woody
        {
            /// <summary>
            /// Wrapper Class to be used with scummvm opl emulation
            /// </summary>
            class WoodyOPL : public scummvm::EmulatedOPL
            {
            public:
                WoodyOPL(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const bool surround);
                ~WoodyOPL() override = default;

                bool init() override;
                void reset() override;
                void write(const uint32_t port, const uint16_t val) noexcept override;
                uint8_t read(const uint32_t port) noexcept override;
                void writeReg(const uint16_t r, const uint16_t v) noexcept override;

            protected:
                void generateSamples(int16_t* buffer, size_t length) noexcept override;

            private:
                std::unique_ptr<woody::OPL> _opl = nullptr;
            };
        }
    }
}
