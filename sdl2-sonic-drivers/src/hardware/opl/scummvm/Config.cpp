#include <hardware/opl/scummvm/Config.hpp>
#include <cstring>
#include <spdlog/spdlog.h>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <hardware/opl/scummvm/dosbox/dosbox.hpp>
#include <hardware/opl/scummvm/nuked/OPL.hpp>
#include <hardware/opl/woody/WoodyOPL.hpp>
#include <hardware/opl/mame/MameOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            std::shared_ptr<OPL> Config::create(OplEmulator oplEmulator, OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer)
            {
                switch (oplEmulator)
                {
                case OplEmulator::MAME:
                    switch (type)
                    {
                    case OplType::OPL2:
                        return std::make_shared<mame::OPL>(mixer);
                    case OplType::DUAL_OPL2:
                        spdlog::warn("MAME OPL emulator doesn't support DUAL_OPL2 emulation");
                        return nullptr;
                    case OplType::OPL3:
                        return std::make_shared<hardware::opl::mame::MameOPL>(mixer);
                    }
                case OplEmulator::AUTO:
                case OplEmulator::DOS_BOX:
                    return std::make_shared<dosbox::OPL>(mixer, type);
                case OplEmulator::NUKED:
                    if (type != OplType::OPL3) {
                        spdlog::warn("Nuke OPL emulator only supports OPL3 emulation");
                        return nullptr;
                    }
                    return std::make_shared<nuked::OPL>(mixer, type);
                case OplEmulator::WOODY:
                    return std::make_shared<woody::WoodyOPL>(mixer, type == OplType::OPL2 ? false : true);
                    
                default:
                    spdlog::error("Unsupported OPL emulator {:d}", static_cast<int>(oplEmulator));
                }

                return nullptr;
            }
        }
    }
}
