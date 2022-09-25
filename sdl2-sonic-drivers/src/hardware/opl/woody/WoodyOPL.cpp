#include <hardware/opl/woody/WoodyOPL.hpp>
#include <hardware/opl/woody/WoodyEmuOPL.hpp>
#include <hardware/opl/woody/SurroundOPL.hpp>
#include <hardware/opl/OplType.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            WoodyOPL::WoodyOPL(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const bool surround)
                : EmulatedOPL(surround ? OplType::DUAL_OPL2 : OplType::OPL2, mixer)
            {}

            bool WoodyOPL::init()
            {
                stop();
                if (type == OplType::DUAL_OPL2)
                    _opl =std::make_unique<SurroundOPL>(_mixer->getOutputRate());
                else
                    _opl = std::make_unique<WoodyEmuOPL>(_mixer->getOutputRate());

                _init = _opl != nullptr;
                if (!_init)
                    return false;

                _opl->init();

                return true;
            }
            void WoodyOPL::reset()
            {
                init();
            }
            void WoodyOPL::write(const uint32_t port, const uint16_t val) noexcept
            {
                _opl->write(port, static_cast<uint8_t>(val));
            }

            uint8_t WoodyOPL::read(const uint32_t port) noexcept
            {
                return _opl->read(port);
            }
            void WoodyOPL::writeReg(const uint16_t r, const uint16_t v) noexcept
            {
                _opl->writeReg(r, v);
            }

            void WoodyOPL::generateSamples(int16_t* buffer, int length) noexcept
            {
                const int d = isStereo() ? 2 : 1;
                _opl->update(buffer, length / d);
            }
        }
    }
}
