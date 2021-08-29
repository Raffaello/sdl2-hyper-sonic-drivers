#include <hardware/opl/woody/WoodyOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            WoodyOPL::WoodyOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer)
                : EmulatedOPL(mixer), _opl(nullptr)
            {
                // TODO add to the mixer the depth in bits (8 bits or 16 bits)

            }

            WoodyOPL::~WoodyOPL()
            {
                free();
            }

            bool WoodyOPL::init()
            {
                free();

                _opl = new WoodyEmuOPL(_mixer->getOutputRate(), true);
                if (_opl == nullptr)
                    return false;

                _opl->init();

                return true;
            }

            void WoodyOPL::reset()
            {
                init();
            }
            void WoodyOPL::write(int a, int v)
            {
                //opl->write(a, v);
            }

            uint8_t WoodyOPL::read(int a)
            {
                return 0;
            }

            void WoodyOPL::writeReg(int r, int v)
            {
                _opl->write(r, v);
            }

            bool WoodyOPL::isStereo() const
            {
                return true;
            }

            void WoodyOPL::generateSamples(int16_t* buffer, int numSamples)
            {
                const int d = isStereo() ? 2 : 1;
                _opl->update(buffer, numSamples / d);
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

