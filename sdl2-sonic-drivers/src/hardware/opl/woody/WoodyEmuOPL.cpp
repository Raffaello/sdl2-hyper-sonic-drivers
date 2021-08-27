#include "WoodyEmuOPL.hpp"

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            WoodyEmuOPL::WoodyEmuOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer, const bool stereo)
                : EmulatedOPL(mixer),
                _opl(mixer->getOutputRate()),
                _type(scummvm::Config::OplType::OPL3),
                _stereo(stereo)
            {
            }

            WoodyEmuOPL::~WoodyEmuOPL()
            {
            }

            void WoodyEmuOPL::generateSamples(int16_t* buffer, int numSamples)
            {
                if (_stereo) {
                    _opl.adlib_getsample(buffer, numSamples * 2);
                } else {
                    _opl.adlib_getsample(buffer, numSamples);
                }
            }

            bool WoodyEmuOPL::init()
            {
                return true;
            }

            void WoodyEmuOPL::reset()
            {
            }

            void WoodyEmuOPL::write(int a, int v)
            {
                _opl.adlib_write(_opl.index, v, 0);
            }

            uint8_t WoodyEmuOPL::read(int a)
            {
                return uint8_t();
            }

            // template methods
            void WoodyEmuOPL::writeReg(const int reg, const int val)
            {
                //if (_currentChip != 0) {
                //    return;
                //}

                _opl.index = reg;
                _opl.adlib_write(_opl.index, val, 0);
            }

            /*int32_t WoodyEmuOPL::getSampleRate() const noexcept
            {
                return _opl.getSampleRate();
            }*/

            bool WoodyEmuOPL::isStereo() const
            {
                return _stereo;
            }
        }
    }
}
