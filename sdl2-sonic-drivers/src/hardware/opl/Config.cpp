#include <hardware/opl/Config.hpp>
#include <cstring>
#include <spdlog/spdlog.h>
#include <hardware/opl/scummvm/mame/MameOPL2.hpp>
#include <hardware/opl/scummvm/dosbox/DosBoxOPL.hpp>
#include <hardware/opl/scummvm/nuked/NukedOPL3.hpp>
#include <hardware/opl/woody/WoodyOPL.hpp>
#include <hardware/opl/mame/MameOPL.hpp>

namespace hardware::opl
{
    std::shared_ptr<OPL> Config::create(OplEmulator oplEmulator, OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer)
    {
        switch (oplEmulator)
        {
        case OplEmulator::MAME:
            switch (type)
            {
            case OplType::OPL2:
                return std::make_shared<scummvm::mame::MameOPL2>(type, mixer);
            case OplType::DUAL_OPL2:
                spdlog::warn("MameOPL2 emulator doesn't support DUAL_OPL2 emulation");
                return nullptr;
            case OplType::OPL3:
                spdlog::warn("MameOPL3 not working yet.");
                //return std::make_shared<hardware::opl::mame::MameOPL>(mixer);
                return nullptr;
            }
        case OplEmulator::AUTO:
        case OplEmulator::DOS_BOX:
            return std::make_shared<scummvm::dosbox::DosBoxOPL>(type, mixer);
        case OplEmulator::NUKED:
            if (type != OplType::OPL3) {
                spdlog::warn("NukedOPL3 emulator only supports OPL3 emulation, disabled for OPL2 and DUAL_OPL2");
               return nullptr;
            }
            return std::make_shared<scummvm::nuked::NukedOPL>(type, mixer);
        case OplEmulator::WOODY:
            return std::make_shared<woody::WoodyOPL>(type, mixer, type == OplType::OPL2 ? false : true);

        default:
            spdlog::error("Unsupported OPL emulator {:d}", static_cast<int>(oplEmulator));
        }

        return nullptr;
    }
}
