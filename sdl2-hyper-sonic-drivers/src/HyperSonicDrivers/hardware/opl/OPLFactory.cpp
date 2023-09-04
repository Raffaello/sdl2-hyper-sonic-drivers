#include <cstring>
#include <format>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/mame/MameOPL2.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/dosbox/DosBoxOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/nuked/NukedOPL3.hpp>
#include <HyperSonicDrivers/hardware/opl/woody/WoodyOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/mame/MameOPL3.hpp>
#include <std/OplEmulatorFormatter.hpp>

#include <SDL2/SDL_log.h>

namespace HyperSonicDrivers::hardware::opl
{
    std::shared_ptr<OPL> OPLFactory::create(OplEmulator oplEmulator, OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer)
    {
        switch (oplEmulator)
        {
        case OplEmulator::MAME:
            switch (type)
            {
            case OplType::OPL2:
                return std::make_shared<scummvm::mame::MameOPL2>(type, mixer);
            case OplType::DUAL_OPL2:
                SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "MameOPL2 emulator doesn't support DUAL_OPL2 emulation");
                return nullptr;
            case OplType::OPL3:
                SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "MameOPL3 not working yet.");
                //return std::make_shared<hardware::opl::mame::MameOPL>(mixer);
                return nullptr;
            }
        case OplEmulator::AUTO:
        case OplEmulator::DOS_BOX:
            return std::make_shared<scummvm::dosbox::DosBoxOPL>(type, mixer);
        case OplEmulator::NUKED:
            if (type != OplType::OPL3) {
                SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "NukedOPL3 emulator only supports OPL3 emulation, disabled for OPL2 and DUAL_OPL2");
               return nullptr;
            }
            return std::make_shared<scummvm::nuked::NukedOPL>(type, mixer);
        case OplEmulator::WOODY:
            if (type == OplType::OPL3) {
                SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "Woody doesn't support OPL3");
                return nullptr;
            }
            return std::make_shared<woody::WoodyOPL>(mixer, type == OplType::OPL2 ? false : true);

        default:
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, std::format("Unsupported OPL emulator {}", oplEmulator).c_str());
        }

        return nullptr;
    }
}
