#include <drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <drivers/midi/scummvm/AdLibPart.hpp>

#include <utils/algorithms.hpp>
#include <spdlog/spdlog.h>
#include <cassert>
#include <audio/midi/types.hpp>


namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
// TODO: review it / remove / replace / refactor
#define ARRAYSIZE(x) ((int)(sizeof(x) / sizeof(x[0])))

            static const uint8_t g_operator1Offsets[9] = {
                0, 1, 2, 8,
                9, 10, 16, 17,
                18
            };

            static const uint8_t g_operator2Offsets[9] = {
                3, 4, 5, 11,
                12, 13, 19, 20,
                21
            };

            static const AdLibSetParams g_setParamTable[] = {
                {0x40, 0, 63, 63},  // level
                {0xE0, 2, 0, 0},    // unused
                {0x40, 6, 192, 0},  // level key scaling
                {0x20, 0, 15, 0},   // modulator frequency multiple
                {0x60, 4, 240, 15}, // attack rate
                {0x60, 0, 15, 15},  // decay rate
                {0x80, 4, 240, 15}, // sustain level
                {0x80, 0, 15, 15},  // release rate
                {0xE0, 0, 3, 0},    // waveformSelect select
                {0x20, 7, 128, 0},  // amp mod
                {0x20, 6, 64, 0},   // vib
                {0x20, 5, 32, 0},   // eg typ
                {0x20, 4, 16, 0},   // ksr
                {0xC0, 0, 1, 0},    // decay alg
                {0xC0, 1, 14, 0}    // feedback
            };

            static const uint8_t g_paramTable1[16] = {
                29, 28, 27, 0,
                3, 4, 7, 8,
                13, 16, 17, 20,
                21, 30, 31, 0
            };

            static const uint16_t g_maxValTable[16] = {
                0x2FF, 0x1F, 0x7, 0x3F,
                0x0F, 0x0F, 0x0F, 0x3,
                0x3F, 0x0F, 0x0F, 0x0F,
                0x3, 0x3E, 0x1F, 0
            };

            static const uint16_t g_numStepsTable[] = {
                1, 2, 4, 5,
                6, 7, 8, 9,
                10, 12, 14, 16,
                18, 21, 24, 30,
                36, 50, 64, 82,
                100, 136, 160, 192,
                240, 276, 340, 460,
                600, 860, 1200, 1600
            };

            static const uint8_t g_noteFrequencies[] = {
                90, 91, 92, 92, 93, 94, 94, 95,
                96, 96, 97, 98, 98, 99, 100, 101,
                101, 102, 103, 104, 104, 105, 106, 107,
                107, 108, 109, 110, 111, 111, 112, 113,
                114, 115, 115, 116, 117, 118, 119, 120,
                121, 121, 122, 123, 124, 125, 126, 127,
                128, 129, 130, 131, 132, 132, 133, 134,
                135, 136, 137, 138, 139, 140, 141, 142,
                143, 145, 146, 147, 148, 149, 150, 151,
                152, 153, 154, 155, 157, 158, 159, 160,
                161, 162, 163, 165, 166, 167, 168, 169,
                171, 172, 173, 174, 176, 177, 178, 180,
                181, 182, 184, 185, 186, 188, 189, 190,
                192, 193, 194, 196, 197, 199, 200, 202,
                203, 205, 206, 208, 209, 211, 212, 214,
                215, 217, 218, 220, 222, 223, 225, 226,
                228, 230, 231, 233, 235, 236, 238, 240,
                242, 243, 245, 247, 249, 251, 252, 254
            };

            MidiDriver_ADLIB::MidiDriver_ADLIB(const std::shared_ptr<hardware::opl::OPL>& opl, const bool opl3mode)
                : _opl3Mode(opl3mode), _opl(opl)
            {
                unsigned int i;

                for (i = 0; i < ARRAYSIZE(_curNotTable); ++i) {
                    _curNotTable[i] = 0;
                }

                for (i = 0; i < ARRAYSIZE(_parts); ++i) {
                    _parts[i].init(this, static_cast<uint8_t>(i + ((i >= 9) ? 1 : 0)));
                }

                for (i = 0; i < ARRAYSIZE(_channelTable2); ++i) {
                    _channelTable2[i] = 0;
                }

                _percussion.init(this, 9);
            }

            MidiDriver_ADLIB::~MidiDriver_ADLIB()
            {
                if (_isOpen)
                    close();
            }

            int MidiDriver_ADLIB::open()
            {
                if (_isOpen)
                    return 4; //MERR_ALREADY_OPEN;

                _isOpen = true;

                int i;
                AdLibVoice* voice;

                for (i = 0, voice = _voices; i != ARRAYSIZE(_voices); i++, voice++) {
                    voice->_channel = static_cast<uint8_t>(i);
                    voice->_s11a.s10 = &voice->_s10b;
                    voice->_s11b.s10 = &voice->_s10a;
                }

                _opl->init();

                _regCache = (uint8_t*)calloc(256, 1);

                adlibWrite(8, 0x40);
                adlibWrite(0xBD, 0x00);
                if (!_opl3Mode) {
                    adlibWrite(1, 0x20);
                    createLookupTable();
                }
                else {
                    _regCacheSecondary = (uint8_t*)calloc(256, 1);
                    adlibWriteSecondary(5, 1);
                }

                hardware::opl::TimerCallBack cb = std::bind(&MidiDriver_ADLIB::onTimer, this);
                auto p = std::make_shared<hardware::opl::TimerCallBack>(cb);
                _opl->start(p);

                return 0;
            }

            void MidiDriver_ADLIB::close()
            {
                if (!_isOpen)
                    return;
                _isOpen = false;

                // Stop the OPL timer
                _opl->stop();

                for (unsigned int i = 0; i < ARRAYSIZE(_voices); ++i) {
                    if (_voices[i]._part)
                        mcOff(&_voices[i]);
                }

                free(_regCache);
                free(_regCacheSecondary);
            }

            void MidiDriver_ADLIB::send(uint32_t b) {
                send(b & 0xF, b & 0xFFFFFFF0);
            }

            void MidiDriver_ADLIB::send(int8_t chan, uint32_t b) {
                using audio::midi::MIDI_EVENT_type_u;
                using audio::midi::MIDI_EVENT_TYPES_HIGH;
                
                uint8_t param2 = (uint8_t)((b >> 16) & 0xFF);
                uint8_t param1 = (uint8_t)((b >> 8) & 0xFF);
                //uint8_t cmd = (uint8_t)(b & 0xF0);
                MIDI_EVENT_type_u cmd;
                cmd.val = static_cast<uint8_t>(b & 0xFF);

                AdLibPart* part;
                if (chan == 9)
                    part = &_percussion;
                else
                    part = &_parts[chan];

                switch (static_cast<MIDI_EVENT_TYPES_HIGH>(cmd.high)) {
                case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:// Note Off
                    part->noteOff(param1);
                    spdlog::debug("noteOff {} {}", chan, param1);
                    break;
                case MIDI_EVENT_TYPES_HIGH::NOTE_ON: // Note On
                    part->noteOn(param1, param2);
                    spdlog::debug("noteOn {} {}", param1, param2);
                    break;
                case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH: // Aftertouch
                    break; // Not supported.
                case MIDI_EVENT_TYPES_HIGH::CONTROLLER: // Control Change
                    part->controlChange(param1, param2);
                    break;
                case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE: // Program Change
                    part->programChange(param1);
                    break;
                case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH: // Channel Pressure
                    break; // Not supported.
                case MIDI_EVENT_TYPES_HIGH::PITCH_BEND: // Pitch Bend
                    part->pitchBend(static_cast<int16_t>((param1 | (param2 << 7)) - 0x2000));
                    break;
                case MIDI_EVENT_TYPES_HIGH::META_SYSEX: // SysEx
                    // We should never get here! SysEx information has to be
                    // sent via high-level semantic methods.
                    spdlog::warn("MidiDriver_ADLIB: Receiving SysEx command on a send() call");
                    break;

                default:
                    spdlog::warn("MidiDriver_ADLIB: Unknown send() command {0:#x}", cmd.val);
                }
            }

            uint32_t MidiDriver_ADLIB::property(int prop, uint32_t param) {
                switch (prop) {
                case PROP_OLD_ADLIB: // Older games used a different operator volume algorithm
                    _scummSmallHeader = (param > 0);
                    if (_scummSmallHeader) {
                        _timerIncrease = 473;
                        _timerThreshold = 1000;
                    }
                    else {
                        _timerIncrease = 0xD69;
                        _timerThreshold = 0x411B;
                    }
                    return 1;

                case PROP_SCUMM_OPL3: // Sam&Max OPL3 support.
                    _opl3Mode = (param > 0);
                    return 1;

                default:
                    break;
                }

                return 0;
            }

            void MidiDriver_ADLIB::setPitchBendRange(uint8_t channel, unsigned int range) {
                // Not supported in OPL3 mode.
                if (_opl3Mode) {
                    return;
                }

                AdLibPart* part = &_parts[channel];
                part->_pitchBendFactor = range;
                for (AdLibVoice* voice = part->_voice; voice; voice = voice->_next)
                {
                    adlibNoteOn(voice->_channel, voice->_note/* + part->_transposeEff*/,
                        (part->_pitchBend * part->_pitchBendFactor >> 6) + part->_detuneEff);
                }
            }

            void MidiDriver_ADLIB::sysEx_customInstrument(uint8_t channel, uint32_t type, const uint8_t* instr)
            {
                _parts[channel].sysEx_customInstrument(type, instr);
            }

            MidiChannel* MidiDriver_ADLIB::allocateChannel()
            {
                for (unsigned int i = 0; i < ARRAYSIZE(_parts); ++i)
                {
                    AdLibPart* part = &_parts[i];
                    if (!part->_allocated)
                    {
                        part->allocate();
                        return part;
                    }
                }
                return nullptr;
            }

            // All the code brought over from IMuseAdLib

            void MidiDriver_ADLIB::adlibWrite(uint8_t reg, uint8_t value) {
                if (_regCache[reg] == value) {
                    return;
                }
#ifdef DEBUG_ADLIB
                debug(6, "%10d: adlibWrite[%x] = %x", g_tick, reg, value);
#endif
                _regCache[reg] = value;

                _opl->writeReg(reg, value);
            }

            void MidiDriver_ADLIB::adlibWriteSecondary(uint8_t reg, uint8_t value) {
                assert(_opl3Mode);

                if (_regCacheSecondary[reg] == value) {
                    return;
                }
#ifdef DEBUG_ADLIB
                debug(6, "%10d: adlibWriteSecondary[%x] = %x", g_tick, reg, value);
#endif
                _regCacheSecondary[reg] = value;

                _opl->writeReg(reg | 0x100, value);
            }

            void MidiDriver_ADLIB::onTimer() {
                //if (_adlibTimerProc)
                //    (*_adlibTimerProc)(_adlibTimerParam);

                _timerCounter += _timerIncrease;
                while (_timerCounter >= _timerThreshold) {
                    _timerCounter -= _timerThreshold;
#ifdef DEBUG_ADLIB
                    g_tick++;
#endif
                    // Sam&Max's OPL3 driver does not have any timer handling like this.
                    if (_opl3Mode)
                        continue;

                    AdLibVoice* voice = _voices;
                    for (int i = 0; i != ARRAYSIZE(_voices); i++, voice++) {
                        if (!voice->_part)
                            continue;
                        if (voice->_duration && (voice->_duration -= 0x11) <= 0) {
                            mcOff(voice);
                            return;
                        }
                        if (voice->_s10a.active) {
                            mcIncStuff(voice, &voice->_s10a, &voice->_s11a);
                        }
                        if (voice->_s10b.active) {
                            mcIncStuff(voice, &voice->_s10b, &voice->_s11b);
                        }
                    }
                }
            }

            //void MidiDriver_ADLIB::setTimerCallback(void* timerParam, /*Common::TimerManager::TimerProc*/ void* timerProc) {
            //    _adlibTimerProc = timerProc;
            //    _adlibTimerParam = timerParam;
            //}

            void MidiDriver_ADLIB::mcOff(AdLibVoice* voice)
            {
                AdLibVoice* tmp;

                adlibKeyOff(voice->_channel);

                tmp = voice->_prev;

                if (voice->_next)
                    voice->_next->_prev = tmp;
                if (tmp)
                    tmp->_next = voice->_next;
                else
                    voice->_part->_voice = voice->_next;

                voice->_part = nullptr;
            }

            void MidiDriver_ADLIB::mcIncStuff(AdLibVoice* voice, Struct10* s10, Struct11* s11)
            {
                uint8_t code;
                AdLibPart* part = voice->_part;

                code = struct10OnTimer(s10, s11);

                if (code & 1) {
                    switch (s11->param) {
                    case 0:
                        voice->_vol2 = s10->startValue + s11->modifyVal;
                        if (!_scummSmallHeader) {
                            adlibSetParam(voice->_channel, 0,
                                g_volumeTable[g_volumeLookupTable[voice->_vol2]
                                [part->_volEff >> 2]]);
                        }
                        else {
                            adlibSetParam(voice->_channel, 0, voice->_vol2);
                        }
                        break;
                    case 13:
                        voice->_vol1 = s10->startValue + s11->modifyVal;
                        if (voice->_twoChan && !_scummSmallHeader) {
                            adlibSetParam(voice->_channel, 13,
                                g_volumeTable[g_volumeLookupTable[voice->_vol1]
                                [part->_volEff >> 2]]);
                        }
                        else {
                            adlibSetParam(voice->_channel, 13, voice->_vol1);
                        }
                        break;
                    case 30:
                        s11->s10->modWheel = (char)s11->modifyVal;
                        break;
                    case 31:
                        s11->s10->unk3 = (char)s11->modifyVal;
                        break;
                    default:
                        adlibSetParam(voice->_channel, s11->param,
                            s10->startValue + s11->modifyVal);
                        break;
                    }
                }

                if (code & 2 && s11->flag0x10)
                    adlibKeyOnOff(voice->_channel);
            }

            void MidiDriver_ADLIB::adlibKeyOff(int chan) {
                uint8_t reg = chan + 0xB0;
                adlibWrite(reg, adlibGetRegValue(reg) & ~0x20);
                if (_opl3Mode) {
                    adlibWriteSecondary(reg, adlibGetRegValueSecondary(reg) & ~0x20);
                }
            }

            uint8_t MidiDriver_ADLIB::struct10OnTimer(Struct10* s10, Struct11* s11) {
                uint8_t result = 0;
                int i;

                if (s10->count && (s10->count -= 17) <= 0) {
                    s10->active = 0;
                    return 0;
                }

                i = s10->curVal + s10->speedHi;
                s10->speedLoCounter += s10->speedLo;
                if (s10->speedLoCounter >= s10->speedLoMax) {
                    s10->speedLoCounter -= s10->speedLoMax;
                    i += s10->direction;
                }
                if (s10->curVal != i || s10->modWheel != s10->modWheelLast) {
                    s10->curVal = i;
                    s10->modWheelLast = s10->modWheel;
                    i = lookupVolume(i, s10->modWheelLast);
                    if (i != s11->modifyVal) {
                        s11->modifyVal = i;
                        result = 1;
                    }
                }

                if (!--s10->numSteps) {
                    s10->active++;
                    if (s10->active > 4) {
                        if (s10->loop) {
                            s10->active = 1;
                            result |= 2;
                            struct10Setup(s10);
                        }
                        else {
                            s10->active = 0;
                        }
                    }
                    else {
                        struct10Setup(s10);
                    }
                }

                return result;
            }

            void MidiDriver_ADLIB::adlibSetParam(int channel, uint8_t param, int value, bool primary) {
                const AdLibSetParams* as;
                uint8_t reg;

                assert(channel >= 0 && channel < 9);
                assert(!_opl3Mode || (param == 0 || param == 13));

                if (param <= 12) {
                    reg = g_operator2Offsets[channel];
                }
                else if (param <= 25) {
                    param -= 13;
                    reg = g_operator1Offsets[channel];
                }
                else if (param <= 27) {
                    param -= 13;
                    reg = channel;
                }
                else if (param == 28 || param == 29) {
                    if (param == 28)
                        value -= 15;
                    else
                        value -= 383;
                    value *= 16;
                    _channelTable2[channel] = value;
                    adlibPlayNote(channel, _curNotTable[channel] + value);
                    return;
                }
                else {
                    return;
                }

                as = &g_setParamTable[param];
                if (as->inversion)
                    value = as->inversion - value;
                reg += as->registerBase;
                if (primary) {
                    adlibWrite(reg, (adlibGetRegValue(reg) & ~as->mask) | (((uint8_t)value) << as->shift));
                }
                else {
                    adlibWriteSecondary(reg, (adlibGetRegValueSecondary(reg) & ~as->mask) | (((uint8_t)value) << as->shift));
                }
            }

            void MidiDriver_ADLIB::adlibKeyOnOff(int channel) {
                assert(!_opl3Mode);

                uint8_t val;
                uint8_t reg = channel + 0xB0;
                assert(channel >= 0 && channel < 9);

                val = adlibGetRegValue(reg);
                adlibWrite(reg, val & ~0x20);
                adlibWrite(reg, val | 0x20);
            }

            void MidiDriver_ADLIB::struct10Setup(Struct10* s10)
            {
                int b, c, d, e, f, g, h;
                uint8_t t;

                b = s10->unk3;
                f = s10->active - 1;

                t = s10->tableA[f];
                e = g_numStepsTable[g_volumeLookupTable[t & 0x7F][b]];
                if (t & 0x80) {
                    e = randomNr(e);
                }
                if (e == 0)
                    e++;

                s10->numSteps = s10->speedLoMax = e;

                if (f != 2) {
                    c = s10->maxValue;
                    g = s10->startValue;
                    t = s10->tableB[f];
                    d = lookupVolume(c, (t & 0x7F) - 31);
                    if (t & 0x80) {
                        d = randomNr(d);
                    }
                    if (d + g > c) {
                        h = c - g;
                    }
                    else {
                        h = d;
                        if (d + g < 0)
                            h = -g;
                    }
                    h -= s10->curVal;
                }
                else {
                    h = 0;
                }

                s10->speedHi = h / e;
                if (h < 0) {
                    h = -h;
                    s10->direction = -1;
                }
                else {
                    s10->direction = 1;
                }

                s10->speedLo = h % e;
                s10->speedLoCounter = 0;
            }

            void MidiDriver_ADLIB::adlibPlayNote(int channel, int note) {
                uint8_t old, oct, notex;
                int note2;
                int i;

                note2 = (note >> 7) - 4;
                note2 = (note2 < 128) ? note2 : 0;

                oct = (note2 / 12);
                if (oct > 7)
                    oct = 7 << 2;
                else
                    oct <<= 2;
                notex = note2 % 12 + 3;

                old = adlibGetRegValue(channel + 0xB0);
                if (old & 0x20) {
                    old &= ~0x20;
                    if (oct > old) {
                        if (notex < 6) {
                            notex += 12;
                            oct -= 4;
                        }
                    }
                    else if (oct < old) {
                        if (notex > 11) {
                            notex -= 12;
                            oct += 4;
                        }
                    }
                }

                i = (notex << 3) + ((note >> 4) & 0x7);
                adlibWrite(channel + 0xA0, g_noteFrequencies[i]);
                adlibWrite(channel + 0xB0, oct | 0x20);
            }

            int MidiDriver_ADLIB::randomNr(int a) {
                static uint8_t _randSeed = 1;
                if (_randSeed & 1) {
                    _randSeed >>= 1;
                    _randSeed ^= 0xB8;
                }
                else {
                    _randSeed >>= 1;
                }
                return _randSeed * a >> 8;
            }

            void MidiDriver_ADLIB::partKeyOff(AdLibPart* part, uint8_t note)
            {
                for (AdLibVoice* voice = part->_voice; voice; voice = voice->_next)
                {
                    if (voice->_note == note)
                    {
                        if (part->_pedal)
                            voice->_waitForPedal = true;
                        else
                            mcOff(voice);
                    }
                }
            }

            void MidiDriver_ADLIB::partKeyOn(AdLibPart* part, const AdLibInstrument* instr, uint8_t note, uint8_t velocity, const AdLibInstrument* second, uint8_t pan) {
                AdLibVoice* voice;

                voice = allocateVoice(part->_priEff);
                if (!voice)
                    return;

                linkMc(part, voice);
                mcKeyOn(voice, instr, note, velocity, second, pan);
            }

            AdLibVoice* MidiDriver_ADLIB::allocateVoice(uint8_t pri)
            {
                AdLibVoice* ac = nullptr;
                AdLibVoice* best = nullptr;

                for (int i = 0; i < 9; i++)
                {
                    if (++_voiceIndex >= 9)
                        _voiceIndex = 0;
                    ac = &_voices[_voiceIndex];
                    if (!ac->_part)
                        return ac;
                    if (!ac->_next) {
                        if (ac->_part->_priEff <= pri) {
                            pri = ac->_part->_priEff;
                            best = ac;
                        }
                    }
                }

                /* SCUMM V3 games don't have note priorities, first comes wins. */
                if (_scummSmallHeader)
                    return nullptr;

                if (best)
                    mcOff(best);
                return best;
            }

            void MidiDriver_ADLIB::linkMc(AdLibPart* part, AdLibVoice* voice)
            {
                voice->_part = part;
                voice->_next = part->_voice;
                part->_voice = voice;
                voice->_prev = nullptr;

                if (voice->_next)
                    voice->_next->_prev = voice;
            }

            void MidiDriver_ADLIB::mcKeyOn(AdLibVoice* voice, const AdLibInstrument* instr, uint8_t note, uint8_t velocity, const AdLibInstrument* second, uint8_t pan) {
                AdLibPart* part = voice->_part;
                uint8_t vol1, vol2;
                uint8_t secVol1 = 0, secVol2 = 0;

                voice->_twoChan = instr->feedback & 1;
                voice->_note = note;
                voice->_waitForPedal = false;
                voice->_duration = instr->duration;
                if (voice->_duration != 0)
                    voice->_duration *= 63;

                if (!_scummSmallHeader)
                {
                    if (_opl3Mode)
                        vol1 = (instr->modScalingOutputLevel & 0x3F) + (velocity * ((instr->modWaveformSelect >> 3) + 1)) / 64;
                    else
                        vol1 = (instr->modScalingOutputLevel & 0x3F) + g_volumeLookupTable[velocity >> 1][instr->modWaveformSelect >> 2];
                }
                else
                {
                    vol1 = 0x3f - (instr->modScalingOutputLevel & 0x3F);
                }

                if (vol1 > 0x3F)
                    vol1 = 0x3F;
                
                voice->_vol1 = vol1;

                if (!_scummSmallHeader)
                {
                    if (_opl3Mode)
                        vol2 = (instr->carScalingOutputLevel & 0x3F) + (velocity * ((instr->carWaveformSelect >> 3) + 1)) / 64;
                    else
                        vol2 = (instr->carScalingOutputLevel & 0x3F) + g_volumeLookupTable[velocity >> 1][instr->carWaveformSelect >> 2];
                }
                else {
                    vol2 = 0x3f - (instr->carScalingOutputLevel & 0x3F);
                }

                if (vol2 > 0x3F)
                    vol2 = 0x3F;
                voice->_vol2 = vol2;

                if (_opl3Mode)
                {
                    voice->_secTwoChan = second->feedback & 1;
                    secVol1 = (second->modScalingOutputLevel & 0x3F) + (velocity * ((second->modWaveformSelect >> 3) + 1)) / 64;
                    if (secVol1 > 0x3F) {
                        secVol1 = 0x3F;
                    }
                    voice->_secVol1 = secVol1;
                    secVol2 = (second->carScalingOutputLevel & 0x3F) + (velocity * ((second->carWaveformSelect >> 3) + 1)) / 64;
                    if (secVol2 > 0x3F) {
                        secVol2 = 0x3F;
                    }
                    voice->_secVol2 = secVol2;
                }

                if (!_scummSmallHeader)
                {
                    if (!_opl3Mode) {
                        int c = part->_volEff >> 2;
                        vol2 = g_volumeTable[g_volumeLookupTable[vol2][c]];
                        if (voice->_twoChan)
                            vol1 = g_volumeTable[g_volumeLookupTable[vol1][c]];
                    }
                    else {
                        vol2 = g_volumeTable[((vol2 + 1) * part->_volEff) >> 7];
                        secVol2 = g_volumeTable[((secVol2 + 1) * part->_volEff) >> 7];
                        if (voice->_twoChan)
                            vol1 = g_volumeTable[((vol1 + 1) * part->_volEff) >> 7];
                        if (voice->_secTwoChan)
                            secVol1 = g_volumeTable[((secVol1 + 1) * part->_volEff) >> 7];
                    }
                }

                //spdlog::debug("channel {} vol1 {} vol2 {} note {}", voice->_channel, vol1, vol2, note);
                adlibSetupChannel(voice->_channel, instr, vol1, vol2);
                if (!_opl3Mode)
                {
                    adlibNoteOnEx(voice->_channel, /*part->_transposeEff + */note, part->_detuneEff + (part->_pitchBend * part->_pitchBendFactor >> 6));

                    if (instr->flagsA & 0x80) {
                        mcInitStuff(voice, &voice->_s10a, &voice->_s11a, instr->flagsA, &instr->extraA);
                    }
                    else {
                        voice->_s10a.active = 0;
                    }

                    if (instr->flagsB & 0x80) {
                        mcInitStuff(voice, &voice->_s10b, &voice->_s11b, instr->flagsB, &instr->extraB);
                    }
                    else {
                        voice->_s10b.active = 0;
                    }
                }
                else {
                    adlibSetupChannelSecondary(voice->_channel, second, secVol1, secVol2, pan);
                    adlibNoteOnEx(voice->_channel, note, part->_pitchBend >> 1);
                }
            }

            void MidiDriver_ADLIB::adlibSetupChannel(int chan, const AdLibInstrument* instr, uint8_t vol1, uint8_t vol2) {
                assert(chan >= 0 && chan < 9);

                uint8_t channel = g_operator1Offsets[chan];
                adlibWrite(channel + 0x20, instr->modCharacteristic);
                adlibWrite(channel + 0x40, (instr->modScalingOutputLevel | 0x3F) - vol1);
                adlibWrite(channel + 0x60, 0xff & (~instr->modAttackDecay));
                adlibWrite(channel + 0x80, 0xff & (~instr->modSustainRelease));
                adlibWrite(channel + 0xE0, instr->modWaveformSelect);

                channel = g_operator2Offsets[chan];
                adlibWrite(channel + 0x20, instr->carCharacteristic);
                adlibWrite(channel + 0x40, (instr->carScalingOutputLevel | 0x3F) - vol2);
                adlibWrite(channel + 0x60, 0xff & (~instr->carAttackDecay));
                adlibWrite(channel + 0x80, 0xff & (~instr->carSustainRelease));
                adlibWrite(channel + 0xE0, instr->carWaveformSelect);

                adlibWrite((uint8_t)chan + 0xC0, instr->feedback
                    | (_opl3Mode ? 0x30 : 0)
                );
            }

            void MidiDriver_ADLIB::adlibSetupChannelSecondary(int chan, const AdLibInstrument* instr, uint8_t vol1, uint8_t vol2, uint8_t pan) {
                assert(chan >= 0 && chan < 9);
                assert(_opl3Mode);

                uint8_t channel = g_operator1Offsets[chan];
                adlibWriteSecondary(channel + 0x20, instr->modCharacteristic);
                adlibWriteSecondary(channel + 0x40, (instr->modScalingOutputLevel | 0x3F) - vol1);
                adlibWriteSecondary(channel + 0x60, 0xff & (~instr->modAttackDecay));
                adlibWriteSecondary(channel + 0x80, 0xff & (~instr->modSustainRelease));
                adlibWriteSecondary(channel + 0xE0, instr->modWaveformSelect);

                channel = g_operator2Offsets[chan];
                adlibWriteSecondary(channel + 0x20, instr->carCharacteristic);
                adlibWriteSecondary(channel + 0x40, (instr->carScalingOutputLevel | 0x3F) - vol2);
                adlibWriteSecondary(channel + 0x60, 0xff & (~instr->carAttackDecay));
                adlibWriteSecondary(channel + 0x80, 0xff & (~instr->carSustainRelease));
                adlibWriteSecondary(channel + 0xE0, instr->carWaveformSelect);

                // The original uses the following (strange) behavior:
#if 0
                if (instr->feedback | (pan > 64)) {
                    adlibWriteSecondary((uint8_t)chan + 0xC0, 0x20);
                }
                else {
                    adlibWriteSecondary((uint8_t)chan + 0xC0, 0x10);
                }
#else
                adlibWriteSecondary((uint8_t)chan + 0xC0, instr->feedback | ((pan > 64) ? 0x20 : 0x10));
#endif
            }

            uint8_t MidiDriver_ADLIB::adlibGetRegValue(uint8_t reg) const noexcept
            {
                return _regCache[reg];
            }

            uint8_t MidiDriver_ADLIB::adlibGetRegValueSecondary(uint8_t reg) const noexcept
            {
                return _regCacheSecondary[reg];
            }

            void MidiDriver_ADLIB::mcInitStuff(AdLibVoice* voice, Struct10* s10,
                Struct11* s11, uint8_t flags, const InstrumentExtra* ie) {
                AdLibPart* part = voice->_part;
                s11->modifyVal = 0;
                s11->flag0x40 = flags & 0x40;
                s10->loop = flags & 0x20;
                s11->flag0x10 = flags & 0x10;
                s11->param = g_paramTable1[flags & 0xF];
                s10->maxValue = g_maxValTable[flags & 0xF];
                s10->unk3 = 31;
                if (s11->flag0x40) {
                    s10->modWheel = part->_modWheel >> 2;
                }
                else {
                    s10->modWheel = 31;
                }

                switch (s11->param) {
                case 0:
                    s10->startValue = voice->_vol2;
                    break;
                case 13:
                    s10->startValue = voice->_vol1;
                    break;
                case 30:
                    s10->startValue = 31;
                    s11->s10->modWheel = 0;
                    break;
                case 31:
                    s10->startValue = 0;
                    s11->s10->unk3 = 0;
                    break;
                default:
                    s10->startValue = adlibGetRegValueParam(voice->_channel, s11->param);
                }

                struct10Init(s10, ie);
            }

            void MidiDriver_ADLIB::struct10Init(Struct10* s10, const InstrumentExtra* ie) {
                s10->active = 1;
                if (!_scummSmallHeader) {
                    s10->curVal = 0;
                }
                else {
                    s10->curVal = s10->startValue;
                    s10->startValue = 0;
                }
                s10->modWheelLast = 31;
                s10->count = ie->a;
                if (s10->count)
                    s10->count *= 63;
                s10->tableA[0] = ie->b;
                s10->tableA[1] = ie->d;
                s10->tableA[2] = ie->f;
                s10->tableA[3] = ie->g;

                s10->tableB[0] = ie->c;
                s10->tableB[1] = ie->e;
                s10->tableB[2] = 0;
                s10->tableB[3] = ie->h;

                struct10Setup(s10);
            }

            int MidiDriver_ADLIB::adlibGetRegValueParam(int chan, uint8_t param) {
                const AdLibSetParams* as;
                uint8_t val;
                uint8_t channel;

                assert(chan >= 0 && chan < 9);

                if (param <= 12) {
                    channel = g_operator2Offsets[chan];
                }
                else if (param <= 25) {
                    param -= 13;
                    channel = g_operator1Offsets[chan];
                }
                else if (param <= 27) {
                    param -= 13;
                    channel = chan;
                }
                else if (param == 28) {
                    return 0xF;
                }
                else if (param == 29) {
                    return 0x17F;
                }
                else {
                    return 0;
                }

                as = &g_setParamTable[param];
                val = adlibGetRegValue(channel + as->registerBase);
                val &= as->mask;
                val >>= as->shift;
                if (as->inversion)
                    val = as->inversion - val;

                return val;
            }

            void MidiDriver_ADLIB::adlibNoteOn(int chan, uint8_t note, int mod) {
                if (_opl3Mode) {
                    adlibNoteOnEx(chan, note, mod);
                    return;
                }

                assert(chan >= 0 && chan < 9);
                int code = (note << 7) + mod;
                _curNotTable[chan] = code;
                adlibPlayNote(chan, (int16_t)_channelTable2[chan] + code);
            }

            void MidiDriver_ADLIB::adlibNoteOnEx(int chan, uint8_t note, int mod) {
                assert(chan >= 0 && chan < 9);

                if (_opl3Mode) {
                    const int noteAdjusted = note + (mod >> 8) - 7;
                    const int pitchAdjust = (mod >> 5) & 7;

                    adlibWrite(0xA0 + chan, g_noteFrequencies[(noteAdjusted % 12) * 8 + pitchAdjust + 6 * 8]);
                    adlibWriteSecondary(0xA0 + chan, g_noteFrequencies[(noteAdjusted % 12) * 8 + pitchAdjust + 6 * 8]);
                    adlibWrite(0xB0 + chan, (utils::CLIP(noteAdjusted / 12, 0, 7) << 2) | 0x20);
                    adlibWriteSecondary(0xB0 + chan, (utils::CLIP(noteAdjusted / 12, 0, 7) << 2) | 0x20);
                }
                else {
                    int code = (note << 7) + mod;
                    _curNotTable[chan] = code;
                    _channelTable2[chan] = 0;
                    adlibPlayNote(chan, code);
                }
            }
        }
    }
}
