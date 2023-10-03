#include <cstring>
#include <format>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibPart.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    using utils::logW;

    void AdLibPart::init(MidiDriver_ADLIB* owner)
    {
        _owner = owner;
        //programChange(0);
    }

    void AdLibPart::allocate()
    {
        _allocated = true;
    }

    AdLibPart::AdLibPart(const uint8_t channel) :
        MidiChannel(channel)
    {
        memset(&_partInstr, 0, sizeof(_partInstr));
        memset(&_partInstrSecondary, 0, sizeof(_partInstrSecondary));
    }

    void AdLibPart::setInstr(const bool isOpl3) noexcept
    {
        if (isOpl3)
        {
            memcpy(&_partInstr, &g_gmInstrumentsOPL3[program][0], sizeof(AdLibInstrument));
            memcpy(&_partInstrSecondary, &g_gmInstrumentsOPL3[program][1], sizeof(AdLibInstrument));
        }
        else
        {
            memcpy(&_partInstr, &g_gmInstruments[program], sizeof(AdLibInstrument));
        }
    }

    void AdLibPart::setCustomInstr(const AdLibInstrument* instr) noexcept
    {
        memcpy(&_partInstr, instr, sizeof(AdLibInstrument));
    }

    MidiDriver* AdLibPart::device() {
        return _owner;
    }

    void AdLibPart::release()
    {
        _allocated = false;
    }

    void AdLibPart::send(uint32_t b) {
        _owner->send(channel, b);
    }

    /*void AdLibPart::noteOff(uint8_t note) {
#ifdef DEBUG_ADLIB
        debug(6, "%10d: noteOff(%d)", g_tick, note);
#endif
        _owner->partKeyOff(this, note);
    }*/

    /*void AdLibPart::noteOn(uint8_t note, uint8_t velocity) {
#ifdef DEBUG_ADLIB
        debug(6, "%10d: noteOn(%d,%d)", g_tick, note, velocity);
#endif
        _owner->partKeyOn(this, &_partInstr, note, velocity,
            &_partInstrSecondary,
            pan);
    }*/

    //void AdLibPart::programChange(uint8_t program) {
    //    if (program > 127)
    //        return;

    //    program = program;
    //    if (!_owner->m_opl3Mode) {
    //        memcpy(&_partInstr, &g_gmInstruments[program], sizeof(AdLibInstrument));
    //    }
    //    else {
    //        memcpy(&_partInstr, &g_gmInstrumentsOPL3[program][0], sizeof(AdLibInstrument));
    //        memcpy(&_partInstrSecondary, &g_gmInstrumentsOPL3[program][1], sizeof(AdLibInstrument));
    //    }

    //    //spdlog::debug("Program {} {}", _channel, program);
    //}

    //void AdLibPart::pitchBend(int16_t bend)
    //{
    //    pitch = bend;
    //    for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
    //    {
    //        if (!_owner->m_opl3Mode)
    //        {
    //            _owner->adlibNoteOn(voice->_channel, voice->getNote()/* + _transposeEff*/,
    //                (pitch * _pitchBendFactor >> 6) + _detuneEff);
    //        }
    //        else
    //        {
    //            _owner->adlibNoteOn(voice->_channel, voice->getNote(), pitch >> 1);
    //        }
    //    }
    //}

    //void AdLibPart::controlChange(uint8_t control, uint8_t value)
    //{
    //    switch (control)
    //    {
    //    case 0:
    //    case 32:
    //        // Bank select. Not supported
    //        break;
    //    case 1:
    //        modulationWheel(value);
    //        //spdlog::debug("modwheel value {}", value);
    //        break;
    //    case 7:
    //        setVolume(value);
    //        //spdlog::debug("volume value {}", value);
    //        break;
    //    case 10:
    //        panPosition(value);
    //        break;
    //    case 16:
    //        pitchBendFactor(value);
    //        break;
    //    case 17:
    //        detune(value);
    //        break;
    //    case 18:
    //        priority(value);
    //        break;
    //    case 64:
    //        setSustain(value);
    //        break;
    //    case 91:
    //        // Effects level. Not supported.
    //        effectLevel(value);
    //        break;
    //    case 93:
    //        // Chorus level. Not supported.
    //        chorusLevel(value);
    //        break;
    //    case 119:
    //        // Unknown, used in Simon the Sorcerer 2
    //        break;
    //    case 121:
    //        // reset all controllers
    //        modulationWheel(0);
    //        pitchBendFactor(0);
    //        detune(0);
    //        setSustain(false);
    //        break;
    //    case 123:
    //        allNotesOff();
    //        break;
    //    default:
    //        logW(std::format("Unknown control change message {:d} {:d}", control, value));
    //    }
    //}

    /*void AdLibPart::modulationWheel(uint8_t value)
    {
        modulation = value;
        for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
        {
            if (voice->_s10a.active && voice->_s11a.flag0x40)
                voice->_s10a.modWheel = modulation >> 2;
            if (voice->_s10b.active && voice->_s11b.flag0x40)
                voice->_s10b.modWheel = modulation >> 2;
        }
    }*/

    /*void AdLibPart::setVolume(uint8_t value)
    {
        volume = value;
        for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
        {
            if (!_owner->m_opl3Mode)
            {
                _owner->adlibSetParam(voice->_channel, 0, g_volumeTable[g_volumeLookupTable[voice->_vol2][volume >> 2]]);
                if (voice->_twoChan) {
                    _owner->adlibSetParam(voice->_channel, 13, g_volumeTable[g_volumeLookupTable[voice->_vol1][volume >> 2]]);
                }
            }
            else
            {
                _owner->adlibSetParam(voice->_channel, 0, g_volumeTable[((voice->_vol2 + 1) * volume) >> 7], true);
                _owner->adlibSetParam(voice->_channel, 0, g_volumeTable[((voice->_secVol2 + 1) * volume) >> 7], false);
                if (voice->_twoChan) {
                    _owner->adlibSetParam(voice->_channel, 13, g_volumeTable[((voice->_vol1 + 1) * volume) >> 7], true);
                }
                if (voice->_secTwoChan) {
                    _owner->adlibSetParam(voice->_channel, 13, g_volumeTable[((voice->_secVol1 + 1) * volume) >> 7], false);
                }
            }
        }
    }*/

    /*void AdLibPart::panPosition(uint8_t value) {
        pan = value;
    }*/

    //void AdLibPart::pitchBendFactor(uint8_t value)
    //{
    //    // Not supported in OPL3 mode.
    //    if (_owner->m_opl3Mode) {
    //        return;
    //    }

    //    _pitchBendFactor = value;
    //    for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
    //    {
    //        _owner->adlibNoteOn(voice->_channel, voice->getNote()/* + _transposeEff*/,
    //            (pitch * _pitchBendFactor >> 6) + _detuneEff);
    //    }
    //}

    //void AdLibPart::detune(uint8_t value)
    //{
    //    // Sam&Max's OPL3 driver uses this for a completly different purpose. It
    //    // is related to voice allocation. We ignore this for now.
    //    // TODO: We probably need to look how the interpreter side of Sam&Max's
    //    // iMuse version handles all this too. Implementing the driver side here
    //    // would be not that hard.
    //    if (_owner->m_opl3Mode) {
    //        //_maxNotes = value;
    //        return;
    //    }

    //    _detuneEff = value;
    //    for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
    //    {
    //        _owner->adlibNoteOn(voice->_channel, voice->getNote()/* + _transposeEff*/,
    //            (pitch * _pitchBendFactor >> 6) + _detuneEff);
    //    }
    //}

    /*void AdLibPart::priority(uint8_t value) {
        _priEff = value;
    }*/

    /*void AdLibPart::setSustain(const uint8_t value)
    {
        sustain = value;
        if (value != 0) {
            for (AdLibVoice* voice = _voice; voice; voice = voice->_next)
            {
                if (voice->_waitForPedal)
                    _owner->mcOff(voice);
            }
        }
    }*/

    /*void AdLibPart::allNotesOff()
    {
        while (_voice)
            _owner->mcOff(_voice);
    }*/

    //void AdLibPart::sysEx_customInstrument(uint32_t type, const uint8_t* instr)
    //{
    //    // Sam&Max allows for instrument overwrites, but we will not support it
    //    // until we can find any track actually using it.
    //    if (_owner->m_opl3Mode)
    //    {
    //        logW("Used in OPL3 mode, not supported");
    //        return;
    //    }

    //    if (type == static_cast<uint32_t>('ADL ')) {
    //        memcpy(&_partInstr, instr, sizeof(AdLibInstrument));
    //    }
    //}
}
