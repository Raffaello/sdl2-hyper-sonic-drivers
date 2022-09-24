#pragma once

#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/woody/OPL.hpp>
#include <memory>

namespace hardware
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
                virtual ~WoodyOPL();

                virtual bool init() override;
                virtual void reset() override;
                virtual void write(const uint32_t port, const uint16_t val) noexcept override;
                virtual uint8_t read(const uint32_t port) noexcept override;
                virtual void writeReg(const uint16_t r, const uint16_t v) noexcept override;

            protected:
                void generateSamples(int16_t* buffer, int length) noexcept override;

            private:
                std::unique_ptr<woody::OPL> _opl = nullptr;

                void free();
            };
        }
    }
}
