#pragma once

#include <cstdint>
#include <memory>
#include <drivers/midi/scummvm/miles/miles.h>
#include <drivers/midi/scummvm/MidiDriver_Multisource.hpp>
#include <hardware/opl/scummvm/Config.hpp>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
            namespace miles
            {
                // Miles Audio AdLib/OPL3 driver
                //
                // TODO: currently missing: OPL3 4-op voices
                //
                // Special cases (great for testing):
                // - sustain feature is used by Return To Zork (demo) right at the start
                // - sherlock holmes 2 does lots of priority sorts right at the start of the intro

                constexpr int MILES_ADLIB_VIRTUAL_FMVOICES_COUNT_MAX = 20;
                constexpr int MILES_ADLIB_PHYSICAL_FMVOICES_COUNT_MAX = 18;

                struct InstrumentEntry
                {
                    uint8_t bankId;
                    uint8_t patchId;
                    int16_t transposition;
                    uint8_t reg20op1;
                    uint8_t reg40op1;
                    uint8_t reg60op1;
                    uint8_t reg80op1;
                    uint8_t regE0op1;
                    uint8_t reg20op2;
                    uint8_t reg40op2;
                    uint8_t reg60op2;
                    uint8_t reg80op2;
                    uint8_t regE0op2;
                    uint8_t regC0;
                };

                class MidiDriver_Miles_AdLib : public MidiDriver_Multisource
                {
                public:
                    //MidiDriver_Miles_AdLib(InstrumentEntry* instrumentTablePtr, uint16_t instrumentTableCount);
                    MidiDriver_Miles_AdLib(std::shared_ptr<InstrumentEntry> instrumentTablePtr, uint16_t instrumentTableCount);
                    MidiDriver_Miles_AdLib(std::shared_ptr<hardware::opl::OPL> opl, const bool opl3mode);
                    virtual ~MidiDriver_Miles_AdLib();

                    // MidiDriver
                    int open() override;
                    void close() override;
                    void send(uint32_t b) override;
                    void send(int8_t source, uint32_t b) override;
                    void metaEvent(int8_t source, uint8_t type, uint8_t* data, uint16_t length) override;
                    MidiChannel* allocateChannel() override { return nullptr; }
                    MidiChannel* getPercussionChannel() override { return nullptr; }

                    bool isOpen() const override { return _isOpen; }
                    uint32_t getBaseTempo() override { return 1000000 / hardware::opl::DEFAULT_CALLBACK_FREQUENCY; }

                    void stopAllNotes(uint8_t source, uint8_t channel) override;
                    void applySourceVolume(uint8_t source) override;
                    void deinitSource(uint8_t source) override;

                    uint32_t property(int prop, uint32_t param) override;

                    void setVolume(uint8_t volume);

                private:
                    hardware::opl::scummvm::Config::OplType _oplType;
                    uint8_t _modePhysicalFmVoicesCount;
                    uint8_t _modeVirtualFmVoicesCount;
                    bool _modeStereo;

                    // the version of Miles AIL/MSS to emulate
                    MilesVersion _milesVersion;

                    // Structure to hold information about current status of MIDI Channels
                    struct MidiChannelEntry {
                        uint8_t   currentPatchBank;
                        const  InstrumentEntry* currentInstrumentPtr;
                        uint8_t   currentProgram;
                        uint16_t currentPitchBender;
                        uint8_t   currentPitchRange;
                        uint8_t   currentVoiceProtection;

                        uint8_t   currentVolume;
                        uint8_t   currentVolumeExpression;

                        uint8_t   currentPanning;

                        uint8_t   currentModulation;
                        uint8_t   currentSustain;

                        uint8_t   currentActiveVoicesCount;

                        MidiChannelEntry() : currentPatchBank(0),
                            currentInstrumentPtr(nullptr),
                            currentProgram(0),
                            currentPitchBender(MIDI_PITCH_BEND_DEFAULT),
                            currentPitchRange(0),
                            currentVoiceProtection(0),
                            currentVolume(0), currentVolumeExpression(0),
                            currentPanning(0),
                            currentModulation(0),
                            currentSustain(0),
                            currentActiveVoicesCount(0) { }
                    };

                    // Structure to hold information about current status of virtual FM Voices
                    struct VirtualFmVoiceEntry {
                        bool   inUse;
                        uint8_t   actualMidiChannel;

                        const  InstrumentEntry* currentInstrumentPtr;

                        bool   isPhysical;
                        uint8_t   physicalFmVoice;

                        uint16_t currentPriority;

                        uint8_t   currentOriginalMidiNote;
                        uint8_t   currentNote;
                        int16_t  currentTransposition;
                        uint8_t   currentVelocity;

                        bool   sustained;

                        VirtualFmVoiceEntry() : inUse(false),
                            actualMidiChannel(0),
                            currentInstrumentPtr(nullptr),
                            isPhysical(false), physicalFmVoice(0),
                            currentPriority(0),
                            currentOriginalMidiNote(0),
                            currentNote(0),
                            currentTransposition(0),
                            currentVelocity(0),
                            sustained(false) { }
                    };

                    // Structure to hold information about current status of physical FM Voices
                    struct PhysicalFmVoiceEntry {
                        bool   inUse;
                        uint8_t   virtualFmVoice;

                        uint8_t   currentB0hReg;

                        PhysicalFmVoiceEntry() : inUse(false),
                            virtualFmVoice(0),
                            currentB0hReg(0) { }
                    };
                   // hardware::opl::OPL* _opl;
                    std::shared_ptr<hardware::opl::OPL> _opl;
                    int _masterVolume;

                    bool _isOpen;

                    // stores information about all MIDI channels (not the actual OPL FM voice channels!)
                    MidiChannelEntry _midiChannels[MIDI_CHANNEL_COUNT];

                    // stores information about all virtual OPL FM voices
                    VirtualFmVoiceEntry _virtualFmVoices[MILES_ADLIB_VIRTUAL_FMVOICES_COUNT_MAX];

                    // stores information about all physical OPL FM voices
                    PhysicalFmVoiceEntry _physicalFmVoices[MILES_ADLIB_PHYSICAL_FMVOICES_COUNT_MAX];

                    // holds all instruments
                    //InstrumentEntry* _instrumentTablePtr;
                    std::shared_ptr<InstrumentEntry> _instrumentTablePtr;
                    uint16_t           _instrumentTableCount;

                    bool circularPhysicalAssignment;
                    uint8_t circularPhysicalAssignmentFmVoice;

                    void resetData();
                    void resetAdLib();
                    void resetAdLibOperatorRegisters(uint8_t baseRegister, uint8_t value);
                    void resetAdLibFMVoiceChannelRegisters(uint8_t baseRegister, uint8_t value);

                    void setRegister(int reg, int value);
                    void setRegisterStereo(uint8_t reg, uint8_t valueLeft, uint8_t valueRight);

                    int16_t searchFreeVirtualFmVoiceChannel();
                    int16_t searchFreePhysicalFmVoiceChannel();

                    void noteOn(uint8_t midiChannel, uint8_t note, uint8_t velocity);
                    void noteOff(uint8_t midiChannel, uint8_t note);

                    void prioritySort();

                    void releaseFmVoice(uint8_t virtualFmVoice);

                    void releaseSustain(uint8_t midiChannel);

                    void updatePhysicalFmVoice(uint8_t virtualFmVoice, bool keyOn, uint16_t registerUpdateFlags);

                    void controlChange(uint8_t midiChannel, uint8_t controllerNumber, uint8_t controllerValue);
                    void programChange(uint8_t midiChannel, uint8_t patchId);

                    const InstrumentEntry* searchInstrument(uint8_t bankId, uint8_t patchId);

                    void pitchBendChange(uint8_t MIDIchannel, uint8_t parameter1, uint8_t parameter2);

                    void applyControllerDefaults(uint8_t source);
                };
            }
        }
    }
}