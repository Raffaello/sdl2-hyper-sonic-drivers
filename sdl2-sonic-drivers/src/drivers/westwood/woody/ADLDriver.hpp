#pragma once

#include <cstdint>
#include <hardware/opl/woody/OPL.hpp> // TODO: Replace with a generic OPL interface
#include <mutex>
#include <files/ADLFile.hpp>
#include <memory>

namespace drivers
{
    namespace westwood
    {
        namespace woody
        {
            // TODO: move
            constexpr int CALLBACKS_PER_SECOND = 72;


            /// <summary>
            /// Driver for .ADL files and OPL Chips
            /// Originally shuold be the DUNE2 ALFX.DRV file and PCSOUND.DRV
            /// This file was propretary for optimized Westwood .ADL files
            /// and they were not using Miles driver for musics in OPL Chips
            /// as those were only for .XMI files and only used for MT-32/GM
            /// ------------------------------------------------------------
            /// AdLib implementation of the sound output device.
            ///
            /// It uses a sound file format special to EoB I, II, Dune II,
            /// Kyrandia 1 and 2, and LoL.There are slightly different
            /// variants: EoB I uses the oldest format(version 1);
            /// EoB II(version 2), Dune IIand Kyrandia 1 (version 3) have
            /// the same file format(but need different offset adjustments);
            /// Kyrandia 2 and LoL format(version 4) is different again.
            /// </summary>
            class ADLDriver final
            {
            public:
                // AdLibDriver(Audio::Mixer *mixer, int version);
                ADLDriver(hardware::opl::woody::OPL* opl);
                ADLDriver(hardware::opl::woody::OPL* opl, std::shared_ptr<files::ADLFile> adl_file);
                ~ADLDriver();
                void setADLFile(std::shared_ptr<files::ADLFile> adl_file) noexcept;
                void initDriver();
                void setSoundData(uint8_t* data, const uint32_t size); /*override*/;
                void startSound(const int track, const int volume); /*override*/;
                bool isChannelPlaying(const int channel); /*override*/;
                void stopAllChannels(); /*override*/;
                int getSoundTrigger() const; /*override*/ //{ return _soundTrigger; }
                void resetSoundTrigger(); /*override*/ //{ _soundTrigger = 0; }

                void callback();

                // TODO: refactor /remove / replace
                // AudioStream API
                int readBuffer(int16_t* buffer, const int numSamples) {
                    int32_t samplesLeft = numSamples;
                    memset(buffer, 0, sizeof(int16_t) * numSamples);
                    while (samplesLeft) {
                        if (!_samplesTillCallback) {
                            callback();
                            _samplesTillCallback = _samplesPerCallback;
                            _samplesTillCallbackRemainder += _samplesPerCallbackRemainder;
                            if (_samplesTillCallbackRemainder >= CALLBACKS_PER_SECOND) {
                                _samplesTillCallback++;
                                _samplesTillCallbackRemainder -= CALLBACKS_PER_SECOND;
                            }
                        }

                        int32_t render = samplesLeft < _samplesTillCallback ? samplesLeft : _samplesTillCallback;
                        samplesLeft -= render;
                        _samplesTillCallback -= render;
                        _opl->update(buffer, render);
                        buffer += render * 2;
                    }

                    return numSamples;
                }

                void ADLDriver::play(uint8_t track);

                /*
                void setSyncJumpMask(uint16_t mask) override { _syncJumpMask = mask; }

                void setMusicVolume(uint8_t volume) override;
                void setSfxVolume(uint8_t volume) override;
                */

                void setVersion(const uint8_t v); // added in AdPlug

            private:
                // TODO: remove/refactor
                int32_t _samplesPerCallback;
                int32_t _samplesPerCallbackRemainder;
                int32_t _samplesTillCallback;
                int32_t _samplesTillCallbackRemainder;

                // TODO: redundant
                int _numPrograms = 0;
                int _version = -0;
                // ----
                std::shared_ptr<files::ADLFile> _adl_file = nullptr;

                // These variables have not yet been named, but some of them are partly
                // known nevertheless:
                //
                // unk39 - Currently unused, except for updateCallback56()
                // unk40 - Currently unused, except for updateCallback56()

                struct Channel {
                    bool lock; // New to ScummVM
                    uint8_t opExtraLevel2;
                    const uint8_t* dataptr;
                    uint8_t duration;
                    uint8_t repeatCounter;
                    int8_t baseOctave;
                    uint8_t priority;
                    uint8_t dataptrStackPos;
                    const uint8_t* dataptrStack[4];
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

                void setupPrograms();
                void executePrograms();

                void resetAdLibState();
                void writeOPL(const uint8_t reg, const uint8_t val);
                void initChannel(Channel& channel);
                void noteOff(Channel& channel);
                void initAdlibChannel(const uint8_t channel);

                uint16_t _rnd;
                uint16_t getRandomNr();

                void setupDuration(const uint8_t duration, Channel& channel);
                void setupNote(const uint8_t rawNote, Channel& channel, const bool flag = false);
                void setupInstrument(uint8_t regOffset, const uint8_t* dataptr, Channel& channel);
                void noteOn(Channel& channel);
                void adjustVolume(Channel& channel);

                void primaryEffectSlide(Channel& channel);
                void primaryEffectVibrato(Channel& channel);
                void secondaryEffect1(Channel& channel);
                uint8_t calculateOpLevel1(Channel& channel);
                uint8_t calculateOpLevel2(Channel& channel);

                static uint16_t checkValue(const int16_t val);

                // The driver uses timer/tempo pairs in several places. On every
                // callback, the tempo is added to the timer. This will frequently
                // cause the timer to "wrap around", which is the signal to go ahead
                // and do more stuff.
                static bool advance(uint8_t& timer, const uint8_t tempo);

                const uint8_t* checkDataOffset(const uint8_t* ptr, const long n);

                // --- TODO: move in ADLFile ----------------------------------
                // The sound data has two lookup tables:
                // * One for programs, starting at offset 0.
                // * One for instruments, starting at offset 300, 500, or 1000.

                // Method moved to parent class in scummvm:
                uint8_t* getProgram(int progId);
                const uint8_t* getInstrument(int instrumentId);
                // ---- END ---------------------------------------------------

                struct ParserOpcode
                {
                    typedef int (ADLDriver::* POpcode)(Channel& channel, const uint8_t* values);
                    POpcode function;
                    const char* name;
                    int values;
                };

                static const ParserOpcode _parserOpcodeTable[];
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

                // These variables have not yet been named, but some of them are partly
                // known nevertheless:
                //
                // _unkTable2[]    - Unknown. Currently only used by updateCallback46()
                // _unkTable2_1[]  - One of the tables in _unkTable2[]
                // _unkTable2_2[]  - One of the tables in _unkTable2[]
                // _unkTable2_3[]  - One of the tables in _unkTable2[]

                int _curChannel = 0;
                uint8_t _soundTrigger = 0;

                uint8_t _beatDivider = 0;
                uint8_t _beatDivCnt = 0;
                uint8_t _callbackTimer = 0xFF;
                uint8_t _beatCounter = 0;
                uint8_t _beatWaiting = 0;
                uint8_t _opLevelBD = 0;
                uint8_t _opLevelHH = 0;
                uint8_t _opLevelSD = 0;
                uint8_t _opLevelTT = 0;
                uint8_t _opLevelCY = 0;
                uint8_t _opExtraLevel1HH = 0;
                uint8_t _opExtraLevel2HH = 0;
                uint8_t _opExtraLevel1CY = 0;
                uint8_t _opExtraLevel2CY = 0;
                uint8_t _opExtraLevel2TT = 0;
                uint8_t _opExtraLevel1TT = 0;
                uint8_t _opExtraLevel1SD = 0;
                uint8_t _opExtraLevel2SD = 0;
                uint8_t _opExtraLevel1BD = 0;
                uint8_t _opExtraLevel2BD = 0;

                // 	OPL::OPL *_adlib;
                //Copl* _opl; // added in AdPlug
                hardware::opl::woody::OPL* _opl;

                uint8_t* _soundData = nullptr; // moved to parent class in scummvm
                uint32_t _soundDataSize = 0; // moved to parent class in scummvm

                struct QueueEntry
                {
                    QueueEntry() : data(0), id(0), volume(0) {}
                    QueueEntry(uint8_t* ptr, uint8_t track, uint8_t vol) : data(ptr), id(track), volume(vol) {}
                    uint8_t* data;
                    uint8_t id;
                    uint8_t volume;
                };

                QueueEntry _programQueue[16];
                int _programStartTimeout = 0;
                int _programQueueStart = 0;
                int _programQueueEnd = 0;
                bool _retrySounds = false;

                void adjustSfxData(uint8_t* ptr, int volume);

                uint8_t* _sfxPointer = nullptr;
                int _sfxPriority;
                int _sfxVelocity;

                Channel _channels[10];

                uint8_t _vibratoAndAMDepthBits = 0;
                uint8_t _rhythmSectionBits = 0;

                uint8_t _curRegOffset = 0;
                uint8_t _tempo = 0;

                const uint8_t* _tablePtr1 = nullptr;
                const uint8_t* _tablePtr2 = nullptr;

                static const uint8_t _regOffset[];
                static const uint16_t _freqTable[];
                static const uint8_t* const _unkTable2[];
                static const int _unkTable2Size;
                static const uint8_t _unkTable2_1[];
                static const uint8_t _unkTable2_2[];
                static const uint8_t _unkTable2_3[];
                static const uint8_t _pitchBendTables[][32];

                uint16_t _syncJumpMask = 0;

                /*
                Common::Mutex _mutex;
                Audio::Mixer *_mixer;
                */
                std::mutex _mutex;

                uint8_t _musicVolume = 0xFF;
                uint8_t _sfxVolume = 0xFF;
            };
        }
    }
}
