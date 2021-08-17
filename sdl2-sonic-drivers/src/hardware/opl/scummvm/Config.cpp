#include <hardware/opl/scummvm/Config.hpp>
#include <cstring>
#include <spdlog/spdlog.h>
#include <hardware/opl/scummvm/mame/mame.hpp>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            const EmulatorDescription* Config::getAvailable()
            {
                return _drivers;
            }

            //DriverId Config::parse(const std::string& name)
            //{
            //    for (int i = 0; _drivers[i].name; ++i) {
            //        // TODO: should be case insensitive...
            //        if (name == _drivers[i].name) {
            //            return _drivers[i].id;
            //        }
            //    }

            //    return DriverId::NONE;
            //}

            const EmulatorDescription* Config::findDriver(DriverId id)
            {
                for (int i = 0; _drivers[i].name; ++i) {
                    if (_drivers[i].id == id) {
                        return &_drivers[i];
                    }
                }

                return nullptr;
            }

            OPL* Config::create(DriverId driver, OplType type, const std::shared_ptr<audio::scummvm::Mixer> mixer)
            {
                switch (driver)
                {
                case OplEmulator::MAME:
                    if (type == OplType::OPL2)
                        return new mame::OPL(mixer);
                    else
                        spdlog::warn("MAME OPL emulator only supports OPL2 emulation");
                    break;
                case OplEmulator::DOS_BOX:
                    //TODO
                    //return new DOSBox::OPL(type);

                case OplEmulator::NUKED:
                    //return new NUKED::OPL(type);
                default:
                    spdlog::warn("Unsupported OPL emulator %d", driver);
                    // TODO: Maybe we should add some dummy emulator too, which just outputs
                    // silence as sound?
                }

                return nullptr;
            }

            //OPL* Config::create(OplType type)
            //{
                //return create(OplEmulator::MAME, type);
            //}

            /*
            DriverId Config::detect(OplType type)
            {
                OplFlags flags = OplFlags::NONE;
                switch (type) {
                case OplType::OPL2:
                    flags = OplFlags::OPL2;
                    break;

                case OplType::DUAL_OPL2:
                    flags = OplFlags::DUAL_OPL2;
                    break;

                case OplType::OPL3:
                    flags = OplFlags::OPL3;
                    break;

                default:
                    break;
                }
                
                DriverId drv = parse(ConfMan.get("opl_driver"));
                if (drv == kAuto) {
                    // Since the "auto" can be explicitly set for a game, and this
                    // driver shows up in the GUI as "<default>", check if there is
                    // a global setting for it before resorting to auto-detection.
                    drv = parse(ConfMan.get("opl_driver", Common::ConfigManager::kApplicationDomain));
                }

                // When a valid driver is selected, check whether it supports
                // the requested OPL chip.
                if (drv != -1 && drv != kAuto) {
                    const EmulatorDescription* driverDesc = findDriver(drv);
                    // If the chip is supported, just use the driver.
                    if (!driverDesc) {
                        warning("The selected OPL driver %d could not be found", drv);
                    }
                    else if ((flags & driverDesc->flags)) {
                        return drv;
                    }
                    else {
                        // Else we will output a warning and just
                        // return that no valid driver is found.
                        warning("Your selected OPL driver \"%s\" does not support type %d emulation, which is requested by your game", _drivers[drv].description, type);
                        return -1;
                    }
                }

                // Detect the first matching emulator
                drv = -1;

                for (int i = 1; _drivers[i].name; ++i) {
                    if (_drivers[i].flags & flags) {
                        drv = _drivers[i].id;
                        break;
                    }
                }

                return drv;
            }
            */
        }
    }
}
