#include <hardware/opl/scummvm/Config.hpp>
#include <cstring>
#include <spdlog/spdlog.h>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <hardware/opl/scummvm/dosbox/dosbox.hpp>
#include <hardware/opl/scummvm/nuked/OPL.hpp>
#include <hardware/opl/woody/WoodyOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            std::shared_ptr<OPL> Config::create(OplEmulator oplEmulator, OplType type, const std::shared_ptr<audio::scummvm::Mixer> mixer)
            {
                switch (oplEmulator)
                {
                case OplEmulator::MAME:
                    if (type != OplType::OPL2)
                        spdlog::warn("MAME OPL emulator only supports OPL2 emulation");
                    
                    return std::make_shared<mame::OPL>(mixer);
                case OplEmulator::AUTO:
                case OplEmulator::DOS_BOX:
                    return std::make_shared<dosbox::OPL>(mixer, type);
                case OplEmulator::NUKED:
                    return std::make_shared<nuked::OPL>(mixer, type);
                case OplEmulator::WOODY:
                    return std::make_shared<woody::WoodyOPL>(mixer, type == OplType::OPL2 ? false : true);
                    
                default:
                    spdlog::error("Unsupported OPL emulator {0:d}", oplEmulator);
                }

                return nullptr;
            }
        }
    }
}
