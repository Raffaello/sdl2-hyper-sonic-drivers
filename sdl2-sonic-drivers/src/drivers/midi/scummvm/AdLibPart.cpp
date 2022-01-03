#include <drivers/midi/scummvm/AdLibPart.hpp>
#include <drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <spdlog/spdlog.h>
#include <cstring>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
            void AdLibPart::init(MidiDriver_ADLIB* owner, uint8_t channel) {
                _owner = owner;
                _channel = channel;
                _priEff = 127;
                programChange(0);
            }

            void AdLibPart::allocate()
            {
                _allocated = true;
            }

            AdLibPart::AdLibPart()
            {
                //_voice = 0;
                //_pitchBend = 0;
                //_pitchBendFactor = 2;
                //_transposeEff = 0;
                //_volEff = 0;
                //_detuneEff = 0;
                //_modWheel = 0;
                //_pedal = 0;
                //_program = 0;
                //_priEff = 0;
                //_pan = 64;

                //_owner = 0;
                //_allocated = false;
                //_channel = 0;

                memset(&_partInstr, 0, sizeof(_partInstr));
                memset(&_partInstrSecondary, 0, sizeof(_partInstrSecondary));
            }

            MidiDriver* AdLibPart::device() {
                return _owner;
            }

            uint8_t AdLibPart::getNumber()
            {
                return _channel;
            }

            void AdLibPart::release()
            {
                _allocated = false;
            }

            void AdLibPart::send(uint32_t b) {
                _owner->send(_channel, b);
            }

            void AdLibPart::noteOff(uint8_t note) {
#ifdef DEBUG_ADLIB
                debug(6, "%10d: noteOff(%d)", g_tick, note);
#endif
                _owner->partKeyOff(this, note);
            }

            void AdLibPart::noteOn(uint8_t note, uint8_t velocity) {
#ifdef DEBUG_ADLIB
                debug(6, "%10d: noteOn(%d,%d)", g_tick, note, velocity);
#endif
                _owner->partKeyOn(this, &_partInstr, note, velocity,
                    & _partInstrSecondary,
                    _pan);
            }

            void AdLibPart::programChange(uint8_t program) {
                if (program > 127)
                    return;

                /*
                unsigned int i;
                unsigned int count = 0;
                for (i = 0; i < ARRAYSIZE(g_gmInstruments[0]); ++i)
                    count += g_gmInstruments[program][i];
                if (!count)
                    warning("No AdLib instrument defined for GM program %d", (int)program);
                */
                _program = program;
                if (!_owner->_opl3Mode) {
                    memcpy(&_partInstr, &g_gmInstruments[program], sizeof(AdLibInstrument));
                }
                else {
                    memcpy(&_partInstr, &g_gmInstrumentsOPL3[program][0], sizeof(AdLibInstrument));
                    memcpy(&_partInstrSecondary, &g_gmInstrumentsOPL3[program][1], sizeof(AdLibInstrument));
                }
            }

            void AdLibPart::pitchBend(int16_t bend)
            {
                _pitchBend = bend;
                for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
                {
                    if (!_owner->_opl3Mode)
                    {
                        _owner->adlibNoteOn(voice->_channel, voice->_note/* + _transposeEff*/,
                            (_pitchBend * _pitchBendFactor >> 6) + _detuneEff);
                    }
                    else
                    {
                        _owner->adlibNoteOn(voice->_channel, voice->_note, _pitchBend >> 1);
                    }
                }
            }

            void AdLibPart::controlChange(uint8_t control, uint8_t value) {
                switch (control) {
                case 0:
                case 32:
                    // Bank select. Not supported
                    break;
                case 1:
                    modulationWheel(value);
                    break;
                case 7:
                    volume(value);
                    break;
                case 10:
                    panPosition(value);
                    break;
                case 16:
                    pitchBendFactor(value);
                    break;
                case 17:
                    detune(value);
                    break;
                case 18:
                    priority(value);
                    break;
                case 64:
                    sustain(value > 0);
                    break;
                case 91:
                    // Effects level. Not supported.
                    break;
                case 93:
                    // Chorus level. Not supported.
                    break;
                case 119:
                    // Unknown, used in Simon the Sorcerer 2
                    break;
                case 121:
                    // reset all controllers
                    modulationWheel(0);
                    pitchBendFactor(0);
                    detune(0);
                    sustain(false);
                    break;
                case 123:
                    allNotesOff();
                    break;
                default:

                    spdlog::warn("AdLib: Unknown control change message {} {}", (int)control, (int)value);
                }
            }

            void AdLibPart::modulationWheel(uint8_t value)
            {
                _modWheel = value;
                for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
                {
                    if (voice->_s10a.active && voice->_s11a.flag0x40)
                        voice->_s10a.modWheel = _modWheel >> 2;
                    if (voice->_s10b.active && voice->_s11b.flag0x40)
                        voice->_s10b.modWheel = _modWheel >> 2;
                }
            }

            void AdLibPart::volume(uint8_t value)
            {
                _volEff = value;
                for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
                {
                    if (!_owner->_opl3Mode)
                    {
                        _owner->adlibSetParam(voice->_channel, 0, g_volumeTable[g_volumeLookupTable[voice->_vol2][_volEff >> 2]]);
                        if (voice->_twoChan) {
                            _owner->adlibSetParam(voice->_channel, 13, g_volumeTable[g_volumeLookupTable[voice->_vol1][_volEff >> 2]]);
                        }
                    }
                    else
                    {
                        _owner->adlibSetParam(voice->_channel, 0, g_volumeTable[((voice->_vol2 + 1) * _volEff) >> 7], true);
                        _owner->adlibSetParam(voice->_channel, 0, g_volumeTable[((voice->_secVol2 + 1) * _volEff) >> 7], false);
                        if (voice->_twoChan) {
                            _owner->adlibSetParam(voice->_channel, 13, g_volumeTable[((voice->_vol1 + 1) * _volEff) >> 7], true);
                        }
                        if (voice->_secTwoChan) {
                            _owner->adlibSetParam(voice->_channel, 13, g_volumeTable[((voice->_secVol1 + 1) * _volEff) >> 7], false);
                        }
                    }
                }
            }

            void AdLibPart::panPosition(uint8_t value) {
                _pan = value;
            }

            void AdLibPart::pitchBendFactor(uint8_t value)
            {
                // Not supported in OPL3 mode.
                if (_owner->_opl3Mode) {
                    return;
                }

                _pitchBendFactor = value;
                for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
                {
                    _owner->adlibNoteOn(voice->_channel, voice->_note/* + _transposeEff*/,
                        (_pitchBend * _pitchBendFactor >> 6) + _detuneEff);
                }
            }

            void AdLibPart::detune(uint8_t value)
            {
                // Sam&Max's OPL3 driver uses this for a completly different purpose. It
                // is related to voice allocation. We ignore this for now.
                // TODO: We probably need to look how the interpreter side of Sam&Max's
                // iMuse version handles all this too. Implementing the driver side here
                // would be not that hard.
                if (_owner->_opl3Mode) {
                    //_maxNotes = value;
                    return;
                }

                _detuneEff = value;
                for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
                {
                    _owner->adlibNoteOn(voice->_channel, voice->_note/* + _transposeEff*/,
                        (_pitchBend * _pitchBendFactor >> 6) + _detuneEff);
                }
            }

            void AdLibPart::priority(uint8_t value) {
                _priEff = value;
            }

            void AdLibPart::sustain(bool value)
            {
                _pedal = value;
                if (!value) {
                    for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
                    {
                        if (voice->_waitForPedal)
                            _owner->mcOff(voice);
                    }
                }
            }

            void AdLibPart::allNotesOff()
            {
                while (_voice)
                    _owner->mcOff(_voice);
            }

            void AdLibPart::sysEx_customInstrument(uint32_t type, const uint8_t* instr)
            {
                // Sam&Max allows for instrument overwrites, but we will not support it
                // until we can find any track actually using it.
                if (_owner->_opl3Mode) {
                    spdlog::warn("AdLibPart::sysEx_customInstrument: Used in OPL3 mode");
                    return;
                }

                if (type == static_cast<uint32_t>('ADL ')) {
                    memcpy(&_partInstr, instr, sizeof(AdLibInstrument));
                }
            }
        }
    }
}
