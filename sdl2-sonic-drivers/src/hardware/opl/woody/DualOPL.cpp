#include <hardware/opl/woody/DualOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            DualOPL::DualOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer)
                : EmulatedOPL(mixer), _opl(nullptr)
            {
                // TODO add to the mixer the depth in bits (8 bits or 16 bits)
                
            }

            DualOPL::~DualOPL()
            {
                free();
            }
            bool DualOPL::init()
            {
                free();

                _opl = new SurroundOPL(_mixer->getOutputRate(), true);
                if (_opl == nullptr)
                    return false;

                _opl->init();

                return true;
            }
            void DualOPL::reset()
            {
                init();
            }
            void DualOPL::write(int a, int v)
            {
                //opl->write(a, v);
            }
            uint8_t DualOPL::read(int a)
            {
                return 0;
            }
            void DualOPL::writeReg(int r, int v)
            {
                _opl->write(r, v);
            }

            bool DualOPL::isStereo() const
            {
                return true;
            }

            void DualOPL::generateSamples(int16_t* buffer, int numSamples)
            {
                const int d = isStereo() ? 2 : 1;
                _opl->update(buffer, numSamples / d);
            }

            void DualOPL::free()
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
