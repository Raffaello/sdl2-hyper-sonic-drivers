#include <hardware/opl/woody/WoodyOPL.hpp>
#include <hardware/opl/woody/WoodyEmuOPL.hpp>
#include <hardware/opl/woody/SurroundOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            WoodyOPL::WoodyOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer, const bool surround)
                : EmulatedOPL(type, mixer), _opl(nullptr), _surround(surround)
            {
            }

            WoodyOPL::~WoodyOPL()
            {
                free();
            }
            bool WoodyOPL::init()
            {
                free();
                if (_surround)
                {
                    _opl = new SurroundOPL(_mixer->getOutputRate(), _mixer->getBitsDepth() == 16);
                }
                else
                {
                    _opl = new WoodyEmuOPL(_mixer->getOutputRate(), false);
                }

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
            void WoodyOPL::write(const int port, const int val) noexcept
            {
                //opl->write(a, v);
            }
            uint8_t WoodyOPL::read(const int port) noexcept
            {
                return 0;
            }
            void WoodyOPL::writeReg(const int r, const int v) noexcept
            {
                _opl->write(r, v);
            }

            bool WoodyOPL::isStereo() const noexcept
            {
                return _surround;
            }

            void WoodyOPL::generateSamples(int16_t* buffer, int length) noexcept
            {
                const int d = isStereo() ? 2 : 1;
                _opl->update(buffer, length / d);
            }

            void WoodyOPL::free()
            {
                stop();
                if (_opl != nullptr) {
                    delete _opl;
                    _opl = nullptr;
                };
            }
        }
    }
}
