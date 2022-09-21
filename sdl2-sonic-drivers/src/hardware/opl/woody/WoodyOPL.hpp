#pragma once

#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/woody/OPL.hpp>

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
                WoodyOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer, const bool surround);
                virtual ~WoodyOPL();

                virtual bool init() override;
                virtual void reset() override;
                virtual void write(const int port, const int val) noexcept override;
                virtual uint8_t read(const int port) noexcept override;
                virtual void writeReg(const int r, const int v) noexcept override;

                bool isStereo() const override;
            protected:
                void generateSamples(int16_t* buffer, int length) noexcept override;

            private:
                woody::OPL* _opl;
                bool _surround;

                void free();
            };
        }
    }
}
