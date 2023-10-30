#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <mutex>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL2instrument.h>
#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/drivers/IMusicDriver.hpp>

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
    class ADLDriver : public IMusicDriver
    {
    public:
        explicit ADLDriver(
            const std::shared_ptr<devices::Opl>& opl,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume = 255,
            const uint8_t pan = 0
        );

        ~ADLDriver() override;

        void setADLFile(const std::shared_ptr<files::westwood::ADLFile>& adl_file) noexcept;

        bool isChannelPlaying(const int channel) const noexcept;
        void stopAllChannels();
        int getSoundTrigger() const;
        void resetSoundTrigger();
        void onCallback();
        void setSyncJumpMask(const uint16_t mask);

        void setOplMusicVolume(const uint8_t volume);
        void setOplSfxVolume(const uint8_t volume);

        void play(const uint8_t track) noexcept override;
        void stop() noexcept override;

        bool isPlaying() const noexcept override;
    private:
        void initDriver_();
        void startSound_(const uint8_t track, const uint8_t volume);

        std::shared_ptr<files::westwood::ADLFile> m_adl_file = nullptr;

        std::shared_ptr<uint8_t[]> m_soundData = nullptr;
        uint32_t m_soundDataSize;

        // The sound data has two lookup tables:
        uint8_t* getProgram_(const int progId) const;
        //const uint8_t* getInstrument_(const int instrumentId) const;
        hardware::opl::OPL2instrument_t getOPL2Instrument_(const int instrumentId) const;
        uint8_t* getProgram_(const int progId, const files::westwood::ADLFile::PROG_TYPE progType) const;

        struct Channel
        {
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

        void primaryEffectSlide_(Channel& channel);
        void primaryEffectVibrato_(Channel& channel);
        void secondaryEffect1_(Channel& channel);

        void resetAdLibState_();
        void writeOPL_(uint8_t reg, uint8_t val);
        void initChannel_(Channel& channel);
        void noteOff_(Channel& channel);
        void initAdlibChannel_(uint8_t num);

        uint16_t getRandomNr_();
        void setupDuration_(const uint8_t duration, Channel& channel);

        void setupNote_(const uint8_t rawNote, Channel& channel, const bool flag = false);
        //void setupInstrument_(uint8_t regOffset, const uint8_t* dataptr, Channel& channel);
        void setupOPL2Instrument_(const uint8_t regOffset, const hardware::opl::OPL2instrument_t& instr, Channel& channel);
        void noteOn_(Channel& channel);

        void adjustVolume_(Channel& channel);

        uint8_t calculateOpLevel1_(const Channel& channel);
        uint8_t calculateOpLevel2_(const Channel& channel);

        static uint16_t checkValue_(const int16_t val);

        // The driver uses timer/tempo pairs in several places. On every
        // callback, the tempo is added to the timer. This will frequently
        // cause the timer to "wrap around", which is the signal to go ahead
        // and do more stuff.
        static bool advance_(uint8_t& timer, const uint8_t tempo);
        const uint8_t* checkDataOffset_(const uint8_t* ptr, long n);

        void setupPrograms_();
        void executePrograms_();

        struct ParserOpcode
        {
            typedef int (ADLDriver::* POpcode)(Channel& channel, const uint8_t* values);
            POpcode function;
            const char* name;
            int values;
        };

        static const std::array<ParserOpcode, 75> m_parserOpcodeTable;
        static const int m_parserOpcodeTableSize;

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
        int m_curChannel = 0;
        uint8_t m_soundTrigger = 0;

        uint16_t m_rnd;

        uint8_t m_beatDivider = 0;
        uint8_t m_beatDivCnt = 0;
        uint8_t m_callbackTimer = 0xFF;
        uint8_t m_beatCounter = 0;
        uint8_t m_beatWaiting = 0;
        uint8_t m_opLevelBD = 0;
        uint8_t m_opLevelHH = 0;
        uint8_t m_opLevelSD = 0;
        uint8_t m_opLevelTT = 0;
        uint8_t m_opLevelCY = 0;
        uint8_t m_opExtraLevel1HH = 0;
        uint8_t m_opExtraLevel2HH = 0;
        uint8_t m_opExtraLevel1CY = 0;
        uint8_t m_opExtraLevel2CY = 0;
        uint8_t m_opExtraLevel2TT = 0;
        uint8_t m_opExtraLevel1TT = 0;
        uint8_t m_opExtraLevel1SD = 0;
        uint8_t m_opExtraLevel2SD = 0;
        uint8_t m_opExtraLevel1BD = 0;
        uint8_t m_opExtraLevel2BD = 0;

        std::shared_ptr<hardware::opl::OPL> m_opl;

        struct QueueEntry
        {
            QueueEntry() : data(0), id(0), volume(0) {}
            QueueEntry(uint8_t* ptr, uint8_t track, uint8_t vol) : data(ptr), id(track), volume(vol) {}
            uint8_t* data;
            uint8_t id;
            uint8_t volume;
        };

        std::array<QueueEntry, 16> m_programQueue;
        int m_programStartTimeout = 0;
        int m_programQueueStart = 0;
        int m_programQueueEnd = 0;
        bool m_retrySounds = false;

        void adjustSfxData(uint8_t* data, int volume);
        uint8_t* m_sfxPointer = nullptr;
        int m_sfxPriority;
        int m_sfxVelocity;

        std::array<Channel, 10>  m_channels;

        uint8_t m_vibratoAndAMDepthBits = 0;
        uint8_t m_rhythmSectionBits = 0;

        uint8_t m_curRegOffset = 0;
        uint8_t m_tempo = 0;

        const uint8_t* m_tablePtr1 = nullptr;
        const uint8_t* m_tablePtr2 = nullptr;

        static const std::array<uint8_t, 9> m_regOffset;
        static const std::array<uint16_t, 12> m_freqTable;
        static const std::array<const uint8_t*, 6> _unkTable2;
        static const int m_unkTable2Size;
        static const uint8_t m_unkTable2_1[];
        static const uint8_t m_unkTable2_2[];
        static const uint8_t m_unkTable2_3[];
        static const uint8_t m_pitchBendTables[][32];

        uint16_t m_syncJumpMask = 0;

        mutable std::mutex m_mutex;

        uint8_t m_oplMusicVolume = 0;
        uint8_t m_oplSfxVolume = 0;

        // Version 1,2,3 possible values, version 3&4 merged into version 3
        uint8_t m_version = 0;
    };
}
