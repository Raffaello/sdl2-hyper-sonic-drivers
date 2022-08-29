#include <hardware/opl/scummvm/dosbox/dosbox.hpp>
#include <hardware/opl/scummvm/dosbox/dbopl/dbopl.hpp>
#include <utils/algorithms.hpp>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <cmath>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            namespace dosbox
            {
                Timer::Timer()
                {
                    masked = false;
                    overflow = false;
                    enabled = false;
                    counter = 0;
                    delay = 0;

                    startTime = 0.0;
                }

                void Timer::update(double time)
                {
                    if (!enabled || !delay)
                        return;
                    double deltaStart = time - startTime;
                    // Only set the overflow flag when not masked
                    if (deltaStart >= 0 && !masked)
                        overflow = 1;
                }

                void Timer::reset(double time)
                {
                    overflow = false;
                    if (!delay || !enabled)
                        return;
                    double delta = (time - startTime);
                    double rem = fmod(delta, delay);
                    double next = delay - rem;
                    startTime = time + next;
                }

                void Timer::stop()
                {
                    enabled = false;
                }

                void Timer::start(double time, int scale)
                {
                    //Don't enable again
                    if (enabled)
                        return;
                    enabled = true;
                    delay = 0.001 * (256 - counter) * scale;
                    startTime = time + delay;
                }

                bool Chip::write(uint32_t reg, uint8_t val)
                {
                    switch (reg)
                    {
                    case 0x02:
                        timer[0].counter = val;
                        return true;
                    case 0x03:
                        timer[1].counter = val;
                        return true;
                    case 0x04:
                    {
                        //double time = g_system->getMillis() / 1000.0;
                        double time = utils::getMillis<uint32_t>() / 1000.0;

                        if (val & 0x80) {
                            timer[0].reset(time);
                            timer[1].reset(time);
                        }
                        else {
                            timer[0].update(time);
                            timer[1].update(time);

                            if (val & 0x1)
                                timer[0].start(time, 80);
                            else
                                timer[0].stop();

                            timer[0].masked = (val & 0x40) > 0;

                            if (timer[0].masked)
                                timer[0].overflow = false;

                            if (val & 0x2)
                                timer[1].start(time, 320);
                            else
                                timer[1].stop();

                            timer[1].masked = (val & 0x20) > 0;

                            if (timer[1].masked)
                                timer[1].overflow = false;
                        }
                    }
                    return true;
                    default:
                        break;
                    }
                    return false;
                }

                uint8_t Chip::read()
                {
                    double time = utils::getMillis<uint32_t>() / 1000.0;

                    timer[0].update(time);
                    timer[1].update(time);

                    uint8_t ret = 0;
                    // Overflow won't be set if a channel is masked
                    if (timer[0].overflow) {
                        ret |= 0x40;
                        ret |= 0x80;
                    }
                    if (timer[1].overflow) {
                        ret |= 0x20;
                        ret |= 0x80;
                    }
                    return ret;
                }

                OPL::OPL(const std::shared_ptr<audio::scummvm::Mixer> mixer, Config::OplType type)
                    : EmulatedOPL(mixer), _type(type), _rate(0), _emulator(nullptr), _reg({ 0 })
                {
                }
               
                OPL::~OPL()
                {
                    stop();
                    free();
                }

                void OPL::free()
                {
                    delete _emulator;
                    _emulator = nullptr;
                }

                bool OPL::init()
                {
                    _init = false;
                    free();

                    memset(&_reg, 0, sizeof(_reg));
                    utils::ARRAYCLEAR(_chip);

                    _emulator = new dbopl::Chip();
                    if (!_emulator)
                        return false;

                    dbopl::InitTables();
                    _rate = _mixer->getOutputRate();
                    _emulator->Setup(_rate);

                    if (_type == Config::OplType::DUAL_OPL2) {
                        // Setup opl3 mode in the hander
                        _emulator->WriteReg(0x105, 1);
                    }

                    _init = true;
                    return true;
                }

                void OPL::reset() {
                    init();
                }

                void OPL::write(int port, int val)
                {
                    if (port & 1)
                    {
                        switch (_type)
                        {
                        case Config::OplType::OPL2:
                        case Config::OplType::OPL3:
                            if (!_chip[0].write(_reg.normal, val))
                                _emulator->WriteReg(_reg.normal, val);
                            break;
                        case Config::OplType::DUAL_OPL2:
                            // Not a 0x??8 port, then write to a specific port
                            if (!(port & 0x8)) {
                                uint8_t index = (port & 2) >> 1;
                                dualWrite(index, _reg.dual[index], val);
                            }
                            else {
                                //Write to both ports
                                dualWrite(0, _reg.dual[0], val);
                                dualWrite(1, _reg.dual[1], val);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    else {
                        // Ask the handler to write the address
                        // Make sure to clip them in the right range
                        switch (_type) {
                        case Config::OplType::OPL2:
                            _reg.normal = _emulator->WriteAddr(port, val) & 0xff;
                            break;
                        case Config::OplType::OPL3:
                            _reg.normal = _emulator->WriteAddr(port, val) & 0x1ff;
                            break;
                        case Config::OplType::DUAL_OPL2:
                            // Not a 0x?88 port, when write to a specific side
                            if (!(port & 0x8)) {
                                uint8_t index = (port & 2) >> 1;
                                _reg.dual[index] = val & 0xff;
                            }
                            else {
                                _reg.dual[0] = val & 0xff;
                                _reg.dual[1] = val & 0xff;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }

                uint8_t OPL::read(int port)
                {
                    switch (_type)
                    {
                    case Config::OplType::OPL2:
                        if (!(port & 1))
                            //Make sure the low bits are 6 on opl2
                            return _chip[0].read() | 0x6;
                        break;
                    case Config::OplType::OPL3:
                        if (!(port & 1))
                            return _chip[0].read();
                        break;
                    case Config::OplType::DUAL_OPL2:
                        // Only return for the lower ports
                        if (port & 1)
                            return 0xff;
                        // Make sure the low bits are 6 on opl2
                        return _chip[(port >> 1) & 1].read() | 0x6;
                    default:
                        break;
                    }
                    return 0;
                }

                void OPL::writeReg(int r, int v)
                {
                    int tempReg = 0;
                    switch (_type)
                    {
                    case Config::OplType::OPL2:
                    case Config::OplType::DUAL_OPL2:
                    case Config::OplType::OPL3:
                        // We can't use _handler->writeReg here directly, since it would miss timer changes.

                        // Backup old setup register
                        tempReg = _reg.normal;

                        // We directly allow writing to secondary OPL3 registers by using
                        // register values >= 0x100.
                        if (_type == Config::OplType::OPL3 && r >= 0x100) {
                            // We need to set the register we want to write to via port 0x222,
                            // since we want to write to the secondary register set.
                            write(0x222, r);
                            // Do the real writing to the register
                            write(0x223, v);
                        }
                        else {
                            // We need to set the register we want to write to via port 0x388
                            write(0x388, r);
                            // Do the real writing to the register
                            write(0x389, v);
                        }

                        // Restore the old register
                        if (_type == Config::OplType::OPL3 && tempReg >= 0x100) {
                            write(0x222, tempReg & ~0x100);
                        }
                        else {
                            write(0x388, tempReg);
                        }
                        break;
                    default:
                        break;
                    };
                }

                bool OPL::isStereo() const
                {
                    return _type != Config::OplType::OPL2;
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

                    // Write to the timer?
                    if (_chip[index].write(reg, val))
                        return;

                    // Enabling panning
                    if (reg >= 0xC0 && reg <= 0xC8) {
                        val &= 15;
                        val |= index ? 0xA0 : 0x50;
                    }

                    uint32_t fullReg = reg + (index ? 0x100 : 0);
                    _emulator->WriteReg(fullReg, val);
                }

                void OPL::generateSamples(int16_t* buffer, int length)
                {
                    // For stereo OPL cards, we divide the sample count by 2,
                    // to match stereo AudioStream behavior.
                    // HACK to check if is opl3, as it has issues working
                    // when OPL3 executing as OPL2
                    if (_type != Config::OplType::OPL2 && _emulator->opl3Active) {
                        length >>= 1;
                    }

                    const unsigned int bufferLength = 512;
                    int32_t tempBuffer[bufferLength * 2];

                    if (_emulator->opl3Active) {
                        while (length > 0) {
                            const unsigned int readSamples = std::min<unsigned int>(length, bufferLength);

                            _emulator->GenerateBlock3(readSamples, tempBuffer);

                            for (unsigned int i = 0; i < (readSamples << 1); ++i)
                                buffer[i] = tempBuffer[i];

                            buffer += (readSamples << 1);
                            length -= readSamples;
                        }
                    }
                    else {
                        while (length > 0) {
                            const unsigned int readSamples = std::min<unsigned int>(length, bufferLength << 1);

                            _emulator->GenerateBlock2(readSamples, tempBuffer);

                            for (unsigned int i = 0; i < readSamples; ++i)
                                buffer[i] = tempBuffer[i];

                            buffer += readSamples;
                            length -= readSamples;
                        }
                    }
                }
            } // End of namespace DOSBox
        }
    } // End of namespace OPL
}
