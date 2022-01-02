#pragma once

#include "mididrv.h"
#include <cstdint>
#include <hardware/opl/OPL.hpp>

#define ENABLE_OPL3

class MidiDriver_ADLIB;
struct AdLibVoice;

struct InstrumentExtra {
#pragma pack(1)
    uint8_t a, b, c, d, e, f, g, h;
};

struct AdLibInstrument {
#pragma pack(1)
    uint8_t modCharacteristic;
    uint8_t modScalingOutputLevel;
    uint8_t modAttackDecay;
    uint8_t modSustainRelease;
    uint8_t modWaveformSelect;
    uint8_t carCharacteristic;
    uint8_t carScalingOutputLevel;
    uint8_t carAttackDecay;
    uint8_t carSustainRelease;
    uint8_t carWaveformSelect;
    uint8_t feedback;
    uint8_t flagsA;
    InstrumentExtra extraA;
    uint8_t flagsB;
    InstrumentExtra extraB;
    uint8_t duration;
};



class AdLibPart : public MidiChannel {
    friend class MidiDriver_ADLIB;

protected:
    //	AdLibPart *_prev, *_next;
    AdLibVoice* _voice;
    int16_t _pitchBend;
    uint8_t _pitchBendFactor;
    //int8_t _transposeEff;
    uint8_t _volEff;
    int8_t _detuneEff;
    uint8_t _modWheel;
    bool _pedal;
    uint8_t _program;
    uint8_t _priEff;
    uint8_t _pan;
    AdLibInstrument _partInstr;
#ifdef ENABLE_OPL3
    AdLibInstrument _partInstrSecondary;
#endif

protected:
    MidiDriver_ADLIB* _owner;
    bool _allocated;
    uint8_t _channel;

    void init(MidiDriver_ADLIB* owner, uint8_t channel);
    void allocate() { _allocated = true; }

public:
    AdLibPart() {
        _voice = 0;
        _pitchBend = 0;
        _pitchBendFactor = 2;
        //_transposeEff = 0;
        _volEff = 0;
        _detuneEff = 0;
        _modWheel = 0;
        _pedal = 0;
        _program = 0;
        _priEff = 0;
        _pan = 64;

        _owner = 0;
        _allocated = false;
        _channel = 0;

        memset(&_partInstr, 0, sizeof(_partInstr));
#ifdef ENABLE_OPL3
        memset(&_partInstrSecondary, 0, sizeof(_partInstrSecondary));
#endif
    }

    MidiDriver* device();
    uint8_t getNumber() { return _channel; }
    void release() { _allocated = false; }

    void send(uint32_t b);

    // Regular messages
    void noteOff(uint8_t note);
    void noteOn(uint8_t note, uint8_t velocity);
    void programChange(uint8_t program);
    void pitchBend(int16_t bend);

    // Control Change messages
    void controlChange(uint8_t control, uint8_t value);
    void modulationWheel(uint8_t value);
    void volume(uint8_t value);
    void panPosition(uint8_t value);
    void pitchBendFactor(uint8_t value);
    void detune(uint8_t value);
    void priority(uint8_t value);
    void sustain(bool value);
    void effectLevel(uint8_t value) { return; } // Not supported
    void chorusLevel(uint8_t value) { return; } // Not supported
    void allNotesOff();

    // SysEx messages
    void sysEx_customInstrument(uint32_t type, const uint8_t* instr);
};

// FYI (Jamieson630)
// It is assumed that any invocation to AdLibPercussionChannel
// will be done through the MidiChannel base class as opposed to the
// AdLibPart base class. If this were NOT the case, all the functions
// listed below would need to be virtual in AdLibPart as well as MidiChannel.
class AdLibPercussionChannel : public AdLibPart {
    friend class MidiDriver_ADLIB;

protected:
    void init(MidiDriver_ADLIB* owner, uint8_t channel);

public:
    ~AdLibPercussionChannel();

    void noteOff(uint8_t note);
    void noteOn(uint8_t note, uint8_t velocity);
    void programChange(uint8_t program) { }

    // Control Change messages
    void modulationWheel(uint8_t value) { }
    void pitchBendFactor(uint8_t value) { }
    void detune(uint8_t value) { }
    void priority(uint8_t value) { }
    void sustain(bool value) { }

    // SysEx messages
    void sysEx_customInstrument(uint32_t type, const uint8_t* instr);

private:
    uint8_t _notes[256];
    AdLibInstrument* _customInstruments[256];
};

struct Struct10 {
    uint8_t active;
    int16_t curVal;
    int16_t count;
    uint16_t maxValue;
    int16_t startValue;
    uint8_t loop;
    uint8_t tableA[4];
    uint8_t tableB[4];
    int8_t unk3;
    int8_t modWheel;
    int8_t modWheelLast;
    uint16_t speedLoMax;
    uint16_t numSteps;
    int16_t speedHi;
    int8_t direction;
    uint16_t speedLo;
    uint16_t speedLoCounter;
};

struct Struct11 {
    int16_t modifyVal;
    uint8_t param, flag0x40, flag0x10;
    Struct10* s10;
};

struct AdLibVoice {
    AdLibPart* _part;
    AdLibVoice* _next, * _prev;
    uint8_t _waitForPedal;
    uint8_t _note;
    uint8_t _channel;
    uint8_t _twoChan;
    uint8_t _vol1, _vol2;
    int16_t _duration;

    Struct10 _s10a;
    Struct11 _s11a;
    Struct10 _s10b;
    Struct11 _s11b;

#ifdef ENABLE_OPL3
    uint8_t _secTwoChan;
    uint8_t _secVol1, _secVol2;
#endif

    AdLibVoice() { memset(this, 0, sizeof(AdLibVoice)); }
};

struct AdLibSetParams {
    uint8_t registerBase;
    uint8_t shift;
    uint8_t mask;
    uint8_t inversion;
};

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



class MidiDriver_ADLIB : public MidiDriver {
    friend class AdLibPart;
    friend class AdLibPercussionChannel;

public:
    MidiDriver_ADLIB(std::shared_ptr<hardware::opl::OPL> opl, const bool opl3mode);

    int open() override;
    void close() override;
    void send(uint32_t b) override;
    void send(int8_t channel, uint32_t b) override; // Supports higher than channel 15
    uint32_t property(int prop, uint32_t param) override;
    bool isOpen() const override { return _isOpen; }
    uint32_t getBaseTempo() override { return 1000000 / hardware::opl::DEFAULT_CALLBACK_FREQUENCY; }

    void setPitchBendRange(uint8_t channel, unsigned int range) override;
    void sysEx_customInstrument(uint8_t channel, uint32_t type, const uint8_t* instr) override;

    MidiChannel* allocateChannel() override;
    MidiChannel* getPercussionChannel() override { return &_percussion; } // Percussion partially supported

    virtual void setTimerCallback(void* timerParam, /*Common::TimerManager::TimerProc*/ void* timerProc);

private:
    bool _scummSmallHeader; // FIXME: This flag controls a special mode for SCUMM V3 games
#ifdef ENABLE_OPL3
    bool _opl3Mode;
#endif

    std::shared_ptr<hardware::opl::OPL> _opl;
    uint8_t* _regCache;
#ifdef ENABLE_OPL3
    uint8_t* _regCacheSecondary;
#endif

    /*Common::TimerManager::TimerProc*/ void* _adlibTimerProc;
    void* _adlibTimerParam;

    int _timerCounter;

    uint16_t _channelTable2[9];
    int _voiceIndex;
    int _timerIncrease;
    int _timerThreshold;
    uint16_t _curNotTable[9];
    AdLibVoice _voices[9];
    AdLibPart _parts[32];
    AdLibPercussionChannel _percussion;

    bool _isOpen;

    void onTimer();
    void partKeyOn(AdLibPart* part, const AdLibInstrument* instr, uint8_t note, uint8_t velocity, const AdLibInstrument* second, uint8_t pan);
    void partKeyOff(AdLibPart* part, uint8_t note);

    void adlibKeyOff(int chan);
    void adlibNoteOn(int chan, uint8_t note, int mod);
    void adlibNoteOnEx(int chan, uint8_t note, int mod);
    int adlibGetRegValueParam(int chan, uint8_t data);
    void adlibSetupChannel(int chan, const AdLibInstrument* instr, uint8_t vol1, uint8_t vol2);
#ifdef ENABLE_OPL3
    void adlibSetupChannelSecondary(int chan, const AdLibInstrument* instr, uint8_t vol1, uint8_t vol2, uint8_t pan);
#endif
    uint8_t adlibGetRegValue(uint8_t reg) {
        return _regCache[reg];
    }
#ifdef ENABLE_OPL3
    uint8_t adlibGetRegValueSecondary(uint8_t reg) {
        return _regCacheSecondary[reg];
    }
#endif
    void adlibSetParam(int channel, uint8_t param, int value, bool primary = true);
    void adlibKeyOnOff(int channel);
    void adlibWrite(uint8_t reg, uint8_t value);
#ifdef ENABLE_OPL3
    void adlibWriteSecondary(uint8_t reg, uint8_t value);
#endif
    void adlibPlayNote(int channel, int note);

    AdLibVoice* allocateVoice(uint8_t pri);

    void mcOff(AdLibVoice* voice);

    static void linkMc(AdLibPart* part, AdLibVoice* voice);
    void mcIncStuff(AdLibVoice* voice, Struct10* s10, Struct11* s11);
    void mcInitStuff(AdLibVoice* voice, Struct10* s10, Struct11* s11, uint8_t flags,
        const InstrumentExtra* ie);

    void struct10Init(Struct10* s10, const InstrumentExtra* ie);
    static uint8_t struct10OnTimer(Struct10* s10, Struct11* s11);
    static void struct10Setup(Struct10* s10);
    static int randomNr(int a);
    void mcKeyOn(AdLibVoice* voice, const AdLibInstrument* instr, uint8_t note, uint8_t velocity, const AdLibInstrument* second, uint8_t pan);
};


