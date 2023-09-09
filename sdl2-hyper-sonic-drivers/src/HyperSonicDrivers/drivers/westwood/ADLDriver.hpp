#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <mutex>
#include <span>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/Adlib.hpp>


namespace HyperSonicDrivers::drivers::westwood
{
    /// <summary>
    /// Driver for .ADL files and OPL Chips
    /// Originally it should be the DUNE2 ALFX.DRV file
    /// (and PCSOUND.DRV instead for fx)
    /// This file was proprietary for optimized Westwood .ADL files
    /// and they were not using Miles driver for music in OPL Chips
    /// as those were only for .XMI files and only used for MT-32/GM
    /// ------------------------------------------------------------
    /// AdLib implementation of the sound output device (OPL2).
    ///
    /// It uses a sound file format special to EoB I, II, Dune II,
    /// Kyrandia 1 and 2, and LoL.There are slightly different
    /// variants: EoB I uses the oldest format (version 1);
    /// EoB II(version 2), Dune II and Kyrandia 1 (version 3) have
    /// the same file format(but need different offset adjustments);
    /// Kyrandia 2 and LoL format(version 4) is different again.
    /// </summary>
    class ADLDriver
    {
    public:
        [[deprecated]]
        explicit ADLDriver(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<files::westwood::ADLFile>& adl_file);
        // NOTE: midi:devices:Adlib, it should receive devices::Adlib instead of OPL, but those are working only with MIDDriver
        //explicit ADLDriver(const midi::devices::Adlib& opl, const std::shared_ptr<files::westwood::ADLFile>& adl_file);
        virtual ~ADLDriver();
        void setADLFile(const std::shared_ptr<files::westwood::ADLFile>& adl_file) noexcept;

        bool isChannelPlaying(const int channel);
        void stopAllChannels();
        int getSoundTrigger() const;
        void resetSoundTrigger();

        void callback();
        void setSyncJumpMask(const uint16_t mask);

        void setMusicVolume(const uint8_t volume);
        void setSfxVolume(const uint8_t volume);

        void play(const uint8_t track, const uint8_t volume);
        bool isPlaying();
    private:
        void initDriver();
        void startSound(const int track, const int volume);

        std::shared_ptr<files::westwood::ADLFile> m_adl_file = nullptr;

        std::shared_ptr<uint8_t[]> m_soundData = nullptr;
        uint32_t m_soundDataSize;

        // The sound data has two lookup tables:
        uint8_t* getProgram(const int progId);
        const uint8_t* getInstrument(const int instrumentId);
        uint8_t* getProgram(const int progId, const files::westwood::ADLFile::PROG_TYPE progType);

        struct Channel {
            bool lock;	// New to ScummVM
            uint8_t opExtraLevel2;
            const uint8_t* dataptr;
            uint8_t duration;
            uint8_t repeatCounter;
            int8_t baseOctave;
            uint8_t priority;
            uint8_t dataptrStackPos;
            std::array<const uint8_t*, 4> dataptrStack;
            int8_t baseNote;
            uint8_t slideTempo;
            uint8_t slideTimer;
            int16_t slideStep;
            int16_t vibratoStep;
            uint8_t vibratoStepRange;
            uint8_t vibratoStepsCountdown;
            uint8_t vibratoNumSteps;
            uint8_t vibratoDelay;
            uint8_t vibratoTempo;
            uint8_t vibratoTimer;
            uint8_t vibratoDelayCountdown;
            uint8_t opExtraLevel1;
            uint8_t spacing2;
            uint8_t baseFreq;
            uint8_t tempo;
            uint8_t timer;
            uint8_t regAx;
            uint8_t regBx;
            typedef void (ADLDriver::* Callback)(Channel&);
            Callback primaryEffect;
            Callback secondaryEffect;
            uint8_t fractionalSpacing;
            uint8_t opLevel1;
            uint8_t opLevel2;
            uint8_t opExtraLevel3;
            uint8_t twoChan;
            uint8_t unk39;
            uint8_t unk40;
            uint8_t spacing1;
            uint8_t durationRandomness;
            uint8_t secondaryEffectTempo;
            uint8_t secondaryEffectTimer;
            int8_t secondaryEffectSize;
            int8_t secondaryEffectPos;
            uint8_t secondaryEffectRegbase;
            uint16_t secondaryEffectData;
            uint8_t tempoReset;
            uint8_t rawNote;
            int8_t pitchBend;
            uint8_t volumeModifier;
        };

        void primaryEffectSlide(Channel& channel);
        void primaryEffectVibrato(Channel& channel);
        void secondaryEffect1(Channel& channel);

        void resetAdLibState();
        void writeOPL(uint8_t reg, uint8_t val);
        void initChannel(Channel& channel);
        void noteOff(Channel& channel);
        void initAdlibChannel(uint8_t num);

        uint16_t getRandomNr();
        void setupDuration(uint8_t duration, Channel& channel);

        void setupNote(uint8_t rawNote, Channel& channel, bool flag = false);
        // TODO: dataptr can be replace with Opl2Instrument_t, it might requires to be mapped
        //       as is organized differently internally in the file.
        void setupInstrument(uint8_t regOffset, const uint8_t* dataptr, Channel& channel);
        void noteOn(Channel& channel);

        void adjustVolume(Channel& channel);

        uint8_t calculateOpLevel1(Channel& channel);
        uint8_t calculateOpLevel2(Channel& channel);

        static uint16_t checkValue(int16_t val);

        // The driver uses timer/tempo pairs in several places. On every
        // callback, the tempo is added to the timer. This will frequently
        // cause the timer to "wrap around", which is the signal to go ahead
        // and do more stuff.
        static bool advance(uint8_t& timer, uint8_t tempo);
        const uint8_t* checkDataOffset(const uint8_t* ptr, long n);

        void setupPrograms();
        void executePrograms();

        struct ParserOpcode
        {
            typedef int (ADLDriver::* POpcode)(Channel& channel, const uint8_t* values);
            POpcode function;
            const char* name;
            int values;
        };

        static const std::array<ParserOpcode, 75> _parserOpcodeTable;
        static const int _parserOpcodeTableSize;

        int update_setRepeat(Channel& channel, const uint8_t* values);
        int update_checkRepeat(Channel& channel, const uint8_t* values);
        int update_setupProgram(Channel& channel, const uint8_t* values);
        int update_setNoteSpacing(Channel& channel, const uint8_t* values);
        int update_jump(Channel& channel, const uint8_t* values);
        int update_jumpToSubroutine(Channel& channel, const uint8_t* values);
        int update_returnFromSubroutine(Channel& channel, const uint8_t* values);
        int update_setBaseOctave(Channel& channel, const uint8_t* values);
        int update_stopChannel(Channel& channel, const uint8_t* values);
        int update_playRest(Channel& channel, const uint8_t* values);
        int update_writeAdLib(Channel& channel, const uint8_t* values);
        int update_setupNoteAndDuration(Channel& channel, const uint8_t* values);
        int update_setBaseNote(Channel& channel, const uint8_t* values);
        int update_setupSecondaryEffect1(Channel& channel, const uint8_t* values);
        int update_stopOtherChannel(Channel& channel, const uint8_t* values);
        int update_waitForEndOfProgram(Channel& channel, const uint8_t* values);
        int update_setupInstrument(Channel& channel, const uint8_t* values);
        int update_setupPrimaryEffectSlide(Channel& channel, const uint8_t* values);
        int update_removePrimaryEffectSlide(Channel& channel, const uint8_t* values);
        int update_setBaseFreq(Channel& channel, const uint8_t* values);
        int update_setupPrimaryEffectVibrato(Channel& channel, const uint8_t* values);
        int update_setPriority(Channel& channel, const uint8_t* values);
        int update_setBeat(Channel& channel, const uint8_t* values);
        int update_waitForNextBeat(Channel& channel, const uint8_t* values);
        int update_setExtraLevel1(Channel& channel, const uint8_t* values);
        int update_setupDuration(Channel& channel, const uint8_t* values);
        int update_playNote(Channel& channel, const uint8_t* values);
        int update_setFractionalNoteSpacing(Channel& channel, const uint8_t* values);
        int update_setTempo(Channel& channel, const uint8_t* values);
        int update_removeSecondaryEffect1(Channel& channel, const uint8_t* values);
        int update_setChannelTempo(Channel& channel, const uint8_t* values);
        int update_setExtraLevel3(Channel& channel, const uint8_t* values);
        int update_setExtraLevel2(Channel& channel, const uint8_t* values);
        int update_changeExtraLevel2(Channel& channel, const uint8_t* values);
        int update_setAMDepth(Channel& channel, const uint8_t* values);
        int update_setVibratoDepth(Channel& channel, const uint8_t* values);
        int update_changeExtraLevel1(Channel& channel, const uint8_t* values);
        int update_clearChannel(Channel& channel, const uint8_t* values);
        int update_changeNoteRandomly(Channel& channel, const uint8_t* values);
        int update_removePrimaryEffectVibrato(Channel& channel, const uint8_t* values);
        int update_pitchBend(Channel& channel, const uint8_t* values);
        int update_resetToGlobalTempo(Channel& channel, const uint8_t* values);
        int update_nop(Channel& channel, const uint8_t* values);
        int update_setDurationRandomness(Channel& channel, const uint8_t* values);
        int update_changeChannelTempo(Channel& channel, const uint8_t* values);
        int updateCallback46(Channel& channel, const uint8_t* values);
        int update_setupRhythmSection(Channel& channel, const uint8_t* values);
        int update_playRhythmSection(Channel& channel, const uint8_t* values);
        int update_removeRhythmSection(Channel& channel, const uint8_t* values);
        int update_setRhythmLevel2(Channel& channel, const uint8_t* values);
        int update_changeRhythmLevel1(Channel& channel, const uint8_t* values);
        int update_setRhythmLevel1(Channel& channel, const uint8_t* values);
        int update_setSoundTrigger(Channel& channel, const uint8_t* values);
        int update_setTempoReset(Channel& channel, const uint8_t* values);
        int updateCallback56(Channel& channel, const uint8_t* values);

    private:
        int _curChannel;
        uint8_t _soundTrigger;

        uint16_t _rnd;

        uint8_t _beatDivider;
        uint8_t _beatDivCnt;
        uint8_t _callbackTimer;
        uint8_t _beatCounter;
        uint8_t _beatWaiting;
        uint8_t _opLevelBD;
        uint8_t _opLevelHH;
        uint8_t _opLevelSD;
        uint8_t _opLevelTT;
        uint8_t _opLevelCY;
        uint8_t _opExtraLevel1HH;
        uint8_t _opExtraLevel2HH;
        uint8_t _opExtraLevel1CY;
        uint8_t _opExtraLevel2CY;
        uint8_t _opExtraLevel2TT;
        uint8_t _opExtraLevel1TT;
        uint8_t _opExtraLevel1SD;
        uint8_t _opExtraLevel2SD;
        uint8_t _opExtraLevel1BD;
        uint8_t _opExtraLevel2BD;

        std::shared_ptr<hardware::opl::OPL> m_opl;

        struct QueueEntry
        {
            QueueEntry() : data(0), id(0), volume(0) {}
            QueueEntry(uint8_t* ptr, uint8_t track, uint8_t vol) : data(ptr), id(track), volume(vol) {}
            uint8_t* data;
            uint8_t id;
            uint8_t volume;
        };

        std::array<QueueEntry, 16> _programQueue;
        int _programStartTimeout;
        int _programQueueStart, _programQueueEnd;
        bool _retrySounds;

        void adjustSfxData(uint8_t* data, int volume);
        uint8_t* _sfxPointer;
        int _sfxPriority;
        int _sfxVelocity;

        std::array<Channel, 10>  m_channels;

        uint8_t _vibratoAndAMDepthBits;
        uint8_t _rhythmSectionBits;

        uint8_t _curRegOffset;
        uint8_t _tempo;

        const uint8_t* _tablePtr1;
        const uint8_t* _tablePtr2;

        static const uint8_t _regOffset[];
        static const uint16_t _freqTable[];
        static const std::array<const uint8_t*, 6> _unkTable2;
        static const int _unkTable2Size;
        static const uint8_t _unkTable2_1[];
        static const uint8_t _unkTable2_2[];
        static const uint8_t _unkTable2_3[];
        static const uint8_t _pitchBendTables[][32];

        uint16_t _syncJumpMask;

        mutable std::mutex m_mutex;

        uint8_t m_musicVolume;
        uint8_t m_sfxVolume;

        // Version 1,2,3 possible values, version 3&4 merged into version 3
        uint8_t m_version = 0;
    };
}
