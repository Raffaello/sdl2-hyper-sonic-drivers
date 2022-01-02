#pragma once

#include <cstdint>

#define ENABLE_OPL3

class MidiDriver_ADLIB;
struct AdLibVoice;

class MidiChannel;
/**
 * Music types that music drivers can implement and engines can rely on.
 */
enum MusicType {
    MT_INVALID = -1,	// Invalid output
    MT_AUTO = 0,		// Auto
    MT_NULL,			// Null
    MT_PCSPK,			// PC Speaker
    MT_PCJR,			// PCjr
    MT_CMS,				// CMS
    MT_ADLIB,			// AdLib
    MT_C64,				// C64
    MT_AMIGA,			// Amiga
    MT_APPLEIIGS,		// Apple IIGS
    MT_TOWNS,			// FM-TOWNS
    MT_PC98,			// PC98
    MT_SEGACD,			// SegaCD
    MT_GM,				// General MIDI
    MT_MT32,			// MT-32
    MT_GS				// Roland GS
};

class MidiDriver_BASE {
public:
    static const uint8_t MIDI_CHANNEL_COUNT = 16;
    static const uint8_t MIDI_RHYTHM_CHANNEL = 9;

    static const uint8_t MIDI_COMMAND_NOTE_OFF = 0x80;
    static const uint8_t MIDI_COMMAND_NOTE_ON = 0x90;
    static const uint8_t MIDI_COMMAND_POLYPHONIC_AFTERTOUCH = 0xA0;
    static const uint8_t MIDI_COMMAND_CONTROL_CHANGE = 0xB0;
    static const uint8_t MIDI_COMMAND_PROGRAM_CHANGE = 0xC0;
    static const uint8_t MIDI_COMMAND_CHANNEL_AFTERTOUCH = 0xD0;
    static const uint8_t MIDI_COMMAND_PITCH_BEND = 0xE0;
    static const uint8_t MIDI_COMMAND_SYSTEM = 0xF0;

    static const uint8_t MIDI_CONTROLLER_BANK_SELECT_MSB = 0x00;
    static const uint8_t MIDI_CONTROLLER_MODULATION = 0x01;
    static const uint8_t MIDI_CONTROLLER_DATA_ENTRY_MSB = 0x06;
    static const uint8_t MIDI_CONTROLLER_VOLUME = 0x07;
    static const uint8_t MIDI_CONTROLLER_PANNING = 0x0A;
    static const uint8_t MIDI_CONTROLLER_EXPRESSION = 0x0B;
    static const uint8_t MIDI_CONTROLLER_BANK_SELECT_LSB = 0x20;
    static const uint8_t MIDI_CONTROLLER_DATA_ENTRY_LSB = 0x26;
    static const uint8_t MIDI_CONTROLLER_SUSTAIN = 0x40;
    static const uint8_t MIDI_CONTROLLER_REVERB = 0x5B;
    static const uint8_t MIDI_CONTROLLER_CHORUS = 0x5D;
    static const uint8_t MIDI_CONTROLLER_RPN_LSB = 0x64;
    static const uint8_t MIDI_CONTROLLER_RPN_MSB = 0x65;
    static const uint8_t MIDI_CONTROLLER_ALL_SOUND_OFF = 0x78;
    static const uint8_t MIDI_CONTROLLER_RESET_ALL_CONTROLLERS = 0x79;
    static const uint8_t MIDI_CONTROLLER_ALL_NOTES_OFF = 0x7B;
    static const uint8_t MIDI_CONTROLLER_OMNI_ON = 0x7C;
    static const uint8_t MIDI_CONTROLLER_OMNI_OFF = 0x7D;
    static const uint8_t MIDI_CONTROLLER_MONO_ON = 0x7E;
    static const uint8_t MIDI_CONTROLLER_POLY_ON = 0x7F;

    static const uint16_t MIDI_RPN_PITCH_BEND_SENSITIVITY = 0x0000;
    static const uint16_t MIDI_RPN_MASTER_TUNING_FINE = 0x0001;
    static const uint16_t MIDI_RPN_MASTER_TUNING_COARSE = 0x0002;
    static const uint16_t MIDI_RPN_NULL = 0x7F7F;

    static const uint8_t MIDI_META_END_OF_TRACK = 0x2F;
    static const uint8_t MIDI_META_SEQUENCER = 0x7F;

    static const uint16_t MIDI_PITCH_BEND_DEFAULT = 0x2000;
    static const uint8_t MIDI_PANNING_DEFAULT = 0x40;
    static const uint8_t MIDI_EXPRESSION_DEFAULT = 0x7F;
    static const uint16_t MIDI_MASTER_TUNING_FINE_DEFAULT = 0x2000;
    static const uint8_t MIDI_MASTER_TUNING_COARSE_DEFAULT = 0x40;

    static const uint8_t GM_PITCH_BEND_SENSITIVITY_DEFAULT = 0x02;

    static const uint8_t GS_RHYTHM_FIRST_NOTE = 0x1B;
    static const uint8_t GS_RHYTHM_LAST_NOTE = 0x58;

    MidiDriver_BASE();

    virtual ~MidiDriver_BASE();

    /**
     * Output a packed midi command to the midi stream.
     * The 'lowest' uint8_t (i.e. b & 0xFF) is the status
     * code, then come (if used) the first and second
     * opcode.
     */
    virtual void send(uint32_t b) = 0;

    /**
     * Send a MIDI command from a specific source. If the MIDI driver
     * does not support multiple sources, the source parameter is
     * ignored.
     */
    virtual void send(int8_t source, uint32_t b) { send(b); }

    /**
     * Output a midi command to the midi stream. Convenience wrapper
     * around the usual 'packed' send method.
     *
     * Do NOT use this for sysEx transmission; instead, use the sysEx()
     * method below.
     */
    void send(uint8_t status, uint8_t firstOp, uint8_t secondOp);

    /**
     * Send a MIDI command from a specific source. If the MIDI driver
     * does not support multiple sources, the source parameter is
     * ignored.
     */
    void send(int8_t source, uint8_t status, uint8_t firstOp, uint8_t secondOp);

    /**
     * Transmit a SysEx to the MIDI device.
     *
     * The given msg MUST NOT contain the usual SysEx frame, i.e.
     * do NOT include the leading 0xF0 and the trailing 0xF7.
     *
     * Furthermore, the maximal supported length of a SysEx
     * is 264 bytes. Passing longer buffers can lead to
     * undefined behavior (most likely, a crash).
     */
    virtual void sysEx(const uint8_t* msg, uint16_t length) { }

    /**
     * Transmit a SysEx to the MIDI device and return the necessary
     * delay until the next SysEx event in milliseconds.
     *
     * This can be used to implement an alternate delay method than the
     * OSystem::delayMillis function used by most sysEx implementations.
     * Note that not every driver needs a delay, or supports this method.
     * In this case, 0 is returned and the driver itself will do a delay
     * if necessary.
     *
     * For information on the SysEx data requirements, see the sysEx method.
     */
    virtual uint16_t sysExNoDelay(const uint8_t* msg, uint16_t length) { sysEx(msg, length); return 0; }

    // TODO: Document this.
    virtual void metaEvent(uint8_t type, uint8_t* data, uint16_t length) { }

    /**
     * Send a meta event from a specific source. If the MIDI driver
     * does not support multiple sources, the source parameter is
     * ignored.
     */
    virtual void metaEvent(int8_t source, uint8_t type, uint8_t* data, uint16_t length) { metaEvent(type, data, length); }

    /**
     * Stops all currently active notes. Specify stopSustainedNotes if
     * the MIDI data makes use of the sustain controller to make sure
     * sustained notes are also stopped.
     *
     * Usually, the MIDI parser tracks active notes and terminates them
     * when playback is stopped. This method should be used as a backup
     * to silence the MIDI output in case the MIDI parser makes a
     * mistake when tracking acive notes. It can also be used when
     * quitting or pausing a game.
     *
     * By default, this method sends an All Notes Off message and, if
     * stopSustainedNotes is true, a Sustain off message on all MIDI
     * channels. Driver implementations can override this if they want
     * to implement this functionality in a different way.
     */
    virtual void stopAllNotes(bool stopSustainedNotes = false);

    /**
     * A driver implementation might need time to prepare playback of
     * a track. Use this function to check if the driver is ready to
     * receive MIDI events.
     */
    virtual bool isReady() { return true; }

protected:

    /**
     * Enables midi dumping to a 'dump.mid' file and to debug messages on screen
     * It's set by '--dump-midi' command line parameter
     */
    bool _midiDumpEnable;

    /** Used for MIDI dumping delta calculation */
    uint32_t _prevMillis;

    /** Stores all MIDI events, will be written to disk after an engine quits */
    std::vector<uint8_t> _midiDumpCache;

    /** Initialize midi dumping mechanism, called only if enabled */
    void midiDumpInit();

    /** Handles MIDI file variable length dumping */
    int midiDumpVarLength(const uint32_t& delta);

    /** Handles MIDI file time delta dumping */
    void midiDumpDelta();

    /** Performs dumping of MIDI commands, called only if enabled */
    void midiDumpDo(uint32_t b);

    /** Performs dumping of MIDI SysEx commands, called only if enabled */
    void midiDumpSysEx(const uint8_t* msg, uint16_t length);

    /** Writes the captured MIDI events to disk, called only if enabled */
    void midiDumpFinish();

};

class MidiDriver : public MidiDriver_BASE {
public:
    /**
     * The device handle.
     *
     * The value 0 is reserved for an invalid device for now.
     * TODO: Maybe we should use -1 (i.e. 0xFFFFFFFF) as
     * invalid device?
     */
    typedef uint32_t DeviceHandle;

    enum DeviceStringType {
        kDriverName,
        kDriverId,
        kDeviceName,
        kDeviceId
    };

    static std::string musicType2GUIO(uint32_t musicType);

    /** Create music driver matching the given device handle, or NULL if there is no match. */
    static MidiDriver* createMidi(DeviceHandle handle);

    /** Returns device handle based on the present devices and the flags parameter. */
    static DeviceHandle detectDevice(int flags);

    /** Find the music driver matching the given driver name/description. */
    static DeviceHandle getDeviceHandle(const std::string& identifier);

    /** Check whether the device with the given handle is available. */
    static bool checkDevice(DeviceHandle handle);

    /** Get the music type matching the given device handle, or MT_AUTO if there is no match. */
    static MusicType getMusicType(DeviceHandle handle);

    /** Get the device description string matching the given device handle and the given type. */
    static std::string getDeviceString(DeviceHandle handle, DeviceStringType type);

    /** Common operations to be done by all drivers on start of send */
    void midiDriverCommonSend(uint32_t b);

    /** Common operations to be done by all drivers on start of sysEx */
    void midiDriverCommonSysEx(const uint8_t* msg, uint16_t length);

private:
    // If detectDevice() detects MT32 and we have a preferred MT32 device
    // we use this to force getMusicType() to return MT_MT32 so that we don't
    // have to rely on the 'True Roland MT-32' config manager setting (since nobody
    // would possibly think about activating 'True Roland MT-32' when he has set
    // 'Music Driver' to '<default>')
    static bool _forceTypeMT32;

public:
    virtual ~MidiDriver() { }

    static const uint8_t _mt32ToGm[128];
    static const uint8_t _gmToMt32[128];

    /**
     * Error codes returned by open.
     * Can be converted to a string with getErrorName().
     */
    enum {
        MERR_CANNOT_CONNECT = 1,
        //		MERR_STREAMING_NOT_AVAILABLE = 2,
        MERR_DEVICE_NOT_AVAILABLE = 3,
        MERR_ALREADY_OPEN = 4
    };

    enum {
        //		PROP_TIMEDIV = 1,
        PROP_OLD_ADLIB = 2,
        PROP_CHANNEL_MASK = 3,
        // HACK: Not so nice, but our SCUMM AdLib code is in audio/
        PROP_SCUMM_OPL3 = 4,
        /**
         * Set this to enable or disable scaling of the MIDI channel
         * volume with the user volume settings (including setting it
         * to 0 when Mute All is selected). This is currently
         * implemented in the MT-32/GM drivers (regular and Miles AIL)
         * and the regular AdLib driver.
         *
         * Default is disabled.
         */
         PROP_USER_VOLUME_SCALING = 5,
         /**
          * Set this property to indicate that the MIDI data used by the
          * game has reversed stereo panning compared to its intended
          * device. The MT-32 has reversed stereo panning compared to
          * the MIDI specification and some game developers chose to
          * stick to the MIDI specification.
          *
          * Do not confuse this with the _midiDeviceReversePanning flag,
          * which indicates that the output MIDI device has reversed
          * stereo panning compared to the intended MIDI device targeted
          * by the MIDI data. This is set by the MT-32/GM driver when
          * MT-32 data is played on a GM device or the other way around.
          * Both flags can be set, which results in no change to the
          * panning.
          *
          * Set this property before opening the driver, to make sure
          * that the default panning is set correctly.
          */
          PROP_MIDI_DATA_REVERSE_PANNING = 6,
          /**
           * Set this property to specify the behavior of the AdLib driver
           * for note frequency and volume calculation.
           *
           * ACCURACY_MODE_SB16_WIN95: volume and frequency calculation is
           * identical to the Windows 95 SB16 driver. This is the default.
           * ACCURACY_MODE_GM: volume and frequency calculation is closer
           * to the General MIDI and MIDI specifications. Volume is more
           * dynamic and frequencies are closer to actual note frequencies.
           * Calculations are more CPU intensive in this mode.
           */
           PROP_OPL_ACCURACY_MODE = 7,
           /**
            * Set this property to specify the OPL channel allocation
            * behavior of the AdLib driver.
            *
            * ALLOCATION_MODE_DYNAMIC: behavior is identical to the Windows
            * 95 SB16 driver. Whenever a note is played, an OPL channel is
            * allocated to play this note if:
            * 1. the channel is not playing a note, or
            * 2. the channel is playing a note of the same instrument, or
            * 3. the channel is playing the least recently started note.
            * This mode is the default.
            * ALLOCATION_MODE_STATIC: when a note is played, an OPL channel
            * is exclusively allocated to the MIDI channel and source
            * playing the note. All notes on this MIDI channel are played
            * using this OPL channel. If no OPL channels are unallocated,
            * allocation will fail and the note will not play. This mode
            * requires MIDI channels to be monophonic (i.e. only play one
            * note at a time).
            */
            PROP_OPL_CHANNEL_ALLOCATION_MODE = 8,
            /**
             * Set this property to specify the Miles AIL/MSS version that the
             * Miles drivers should emulate.
             *
             * MILES_VERSION_2: behavior matches Miles AIL versions 1 and 2.
             * Specifically, GM devices are initialized like the MT-32 because
             * these versions do not yet support GM.
             * MILES_VERSION_3: behavior matches Miles Sound System version 3 and
             * higher. GM devices are initialized according to the GM standard.
             */
             PROP_MILES_VERSION = 9
    };

    /**
     * Open the midi driver.
     * @return 0 if successful, otherwise an error code.
     */
    virtual int open() = 0;

    /**
     * Check whether the midi driver has already been opened.
     */
    virtual bool isOpen() const = 0;

    /** Close the midi driver. */
    virtual void close() = 0;

    /** Get or set a property. */
    virtual uint32_t property(int prop, uint32_t param) { return 0; }

    /** Retrieve a string representation of an error code. */
    static const char* getErrorName(int error_code);

    // HIGH-LEVEL SEMANTIC METHODS
    virtual void setPitchBendRange(uint8_t channel, unsigned int range) {
        send(MIDI_COMMAND_CONTROL_CHANGE | channel, MIDI_CONTROLLER_RPN_MSB, MIDI_RPN_PITCH_BEND_SENSITIVITY >> 8);
        send(MIDI_COMMAND_CONTROL_CHANGE | channel, MIDI_CONTROLLER_RPN_LSB, MIDI_RPN_PITCH_BEND_SENSITIVITY & 0xFF);
        send(MIDI_COMMAND_CONTROL_CHANGE | channel, MIDI_CONTROLLER_DATA_ENTRY_MSB, range); // Semi-tones
        send(MIDI_COMMAND_CONTROL_CHANGE | channel, MIDI_CONTROLLER_DATA_ENTRY_LSB, 0); // Cents
        send(MIDI_COMMAND_CONTROL_CHANGE | channel, MIDI_CONTROLLER_RPN_MSB, MIDI_RPN_NULL >> 8);
        send(MIDI_COMMAND_CONTROL_CHANGE | channel, MIDI_CONTROLLER_RPN_LSB, MIDI_RPN_NULL & 0xFF);
    }

    /**
     * Send a Roland MT-32 reset sysEx to the midi device.
     */
    void sendMT32Reset();

    /**
     * Send a General MIDI reset sysEx to the midi device.
     */
    void sendGMReset();

    virtual void sysEx_customInstrument(uint8_t channel, uint32_t type, const uint8_t* instr) { }

    // Timing functions - MidiDriver now operates timers
    virtual void setTimerCallback(void* timer_param, /*Common::TimerManager::TimerProc*/ void* timer_proc) = 0;

    /** The time in microseconds between invocations of the timer callback. */
    virtual uint32_t getBaseTempo() = 0;

    // Channel allocation functions
    virtual MidiChannel* allocateChannel() = 0;
    virtual MidiChannel* getPercussionChannel() = 0;

    // Allow an engine to supply its own soundFont data. This stream will be destroyed after use.
    virtual void setEngineSoundFont(/*Common::SeekableReadStream*/ void* soundFontData) { }

    // Does this driver accept soundFont data?
    virtual bool acceptsSoundFontData() { return false; }
};




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


class MidiChannel {
public:
    virtual ~MidiChannel() {}

    virtual MidiDriver* device() = 0;
    virtual uint8_t getNumber() = 0;
    virtual void release() = 0;

    virtual void send(uint32_t b) = 0; // 4-bit channel portion is ignored

    // Regular messages
    virtual void noteOff(uint8_t note) = 0;
    virtual void noteOn(uint8_t note, uint8_t velocity) = 0;
    virtual void programChange(uint8_t program) = 0;
    virtual void pitchBend(int16_t bend) = 0; // -0x2000 to +0x1FFF

    // Control Change messages
    virtual void controlChange(uint8_t control, uint8_t value) = 0;
    virtual void modulationWheel(uint8_t value) { controlChange(MidiDriver::MIDI_CONTROLLER_MODULATION, value); }
    virtual void volume(uint8_t value) { controlChange(MidiDriver::MIDI_CONTROLLER_VOLUME, value); }
    virtual void panPosition(uint8_t value) { controlChange(MidiDriver::MIDI_CONTROLLER_PANNING, value); }
    virtual void pitchBendFactor(uint8_t value) = 0;
    virtual void transpose(int8_t value) {}
    virtual void detune(uint8_t value) { controlChange(17, value); }
    virtual void priority(uint8_t value) { }
    virtual void sustain(bool value) { controlChange(MidiDriver::MIDI_CONTROLLER_SUSTAIN, value ? 1 : 0); }
    virtual void effectLevel(uint8_t value) { controlChange(MidiDriver::MIDI_CONTROLLER_REVERB, value); }
    virtual void chorusLevel(uint8_t value) { controlChange(MidiDriver::MIDI_CONTROLLER_CHORUS, value); }
    virtual void allNotesOff() { controlChange(MidiDriver::MIDI_CONTROLLER_ALL_NOTES_OFF, 0); }

    // SysEx messages
    virtual void sysEx_customInstrument(uint32_t type, const uint8_t* instr) = 0;
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
    MidiDriver_ADLIB(std::shared_ptr<hardware::opl::OPL> opl);

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


