#include <hardware/opl/scummvm/nuked/OPL.hpp>
#include <hardware/opl/scummvm/nuked/opl3.h>
#include <cstdlib>
#include <cstring>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            namespace nuked
            {
                OPL::OPL(const std::shared_ptr<audio::scummvm::Mixer> mixer, Config::OplType type)
                    : EmulatedOPL(mixer),
                    _type(type), _rate(0)
                {
                    chip = std::make_unique<opl3_chip>();
                }

                OPL::~OPL()
                {
                    stop();
                }

                bool OPL::init()
                {
                    _rate = _mixer->getOutputRate();
                    OPL3_Reset(chip.get(), _rate);

                    if (_type == Config::OplType::DUAL_OPL2) {
                        OPL3_WriteReg(chip.get(), 0x105, 0x01);
                    }

                    return true;
                }

                void OPL::reset()
                {
                    OPL3_Reset(chip.get(), _rate);
                }

                void OPL::write(int port, int val)
                {
                    if (port & 1) {
                        switch (_type) {
                        case Config::OplType::OPL2:
                        case Config::OplType::OPL3:
                            OPL3_WriteRegBuffered(chip.get(), (uint16_t)address[0], (uint8_t)val);
                            break;
                        case Config::OplType::DUAL_OPL2:
                            // Not a 0x??8 port, then write to a specific port
                            if (!(port & 0x8)) {
                                uint8_t index = (port & 2) >> 1;
                                dualWrite(index, address[index], val);
                            }
                            else {
                                //Write to both ports
                                dualWrite(0, address[0], val);
                                dualWrite(1, address[1], val);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    else {
                        switch (_type) {
                        case Config::OplType::OPL2:
                            address[0] = val & 0xff;
                            break;
                        case Config::OplType::DUAL_OPL2:
                            // Not a 0x?88 port, when write to a specific side
                            if (!(port & 0x8)) {
                                uint8_t index = (port & 2) >> 1;
                                address[index] = val & 0xff;
                            }
                            else {
                                address[0] = val & 0xff;
                                address[1] = val & 0xff;
                            }
                            break;
                        case Config::OplType::OPL3:
                            address[0] = (val & 0xff) | ((port << 7) & 0x100);
                            break;
                        default:
                            break;
                        }
                    }
                }

                void OPL::writeReg(int r, int v)
                {
                    OPL3_WriteRegBuffered(chip.get(), (uint16_t)r, (uint8_t)v);
                }

                void OPL::dualWrite(uint8_t index, uint8_t reg, uint8_t val)
                {
                    // Make sure you don't use opl3 features
                    // Don't allow write to disable opl3
                    if (reg == 5)
                        return;

                    // Only allow 4 waveforms
                    if (reg >= 0xE0 && reg <= 0xE8)
                        val &= 3;

                    // Enabling panning
                    if (reg >= 0xC0 && reg <= 0xC8) {
                        val &= 15;
                        val |= index ? 0xA0 : 0x50;
                    }

                    uint32_t fullReg = reg + (index ? 0x100 : 0);
                    OPL3_WriteRegBuffered(chip.get(), (uint16_t)fullReg, (uint8_t)val);
                }

                uint8_t OPL::read(int port)
                {
                    int i = 0;
                    return 0;
                }

                void OPL::generateSamples(int16_t* buffer, int length) {
                    OPL3_GenerateStream(chip.get(), (int16_t*)buffer, (uint16_t)length / 2);
                }
            }
        }
    }
}
