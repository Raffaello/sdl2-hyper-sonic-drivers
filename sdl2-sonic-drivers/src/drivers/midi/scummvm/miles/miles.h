#pragma once

#include <drivers/midi/scummvm/MidiDriver_Multisource.hpp>
#include <cstdint>
#include <string>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
            namespace miles
            {
     // Miles Audio supported controllers for control change messages
#define MILES_CONTROLLER_SELECT_PATCH_BANK 114
#define MILES_CONTROLLER_PROTECT_VOICE 112
#define MILES_CONTROLLER_PROTECT_TIMBRE 113
#define MILES_CONTROLLER_LOCK_CHANNEL 110
#define MILES_CONTROLLER_PROTECT_CHANNEL 111
#define MILES_CONTROLLER_PITCH_RANGE 6
#define MILES_CONTROLLER_PATCH_REVERB 59
#define MILES_CONTROLLER_PATCH_BENDER 60
#define MILES_CONTROLLER_REVERB_MODE 61
#define MILES_CONTROLLER_REVERB_TIME 62
#define MILES_CONTROLLER_REVERB_LEVEL 63
#define MILES_CONTROLLER_RHYTHM_KEY_TIMBRE 58

// 3 SysEx controllers, each range 5
// 32-36 for 1st queue
// 37-41 for 2nd queue
// 42-46 for 3rd queue
#define MILES_CONTROLLER_SYSEX_RANGE_BEGIN 32
#define MILES_CONTROLLER_SYSEX_RANGE_END 46

#define MILES_CONTROLLER_SYSEX_QUEUE_COUNT 3
#define MILES_CONTROLLER_SYSEX_QUEUE_SIZE 32

#define MILES_CONTROLLER_SYSEX_COMMAND_ADDRESS1   0
#define MILES_CONTROLLER_SYSEX_COMMAND_ADDRESS2   1
#define MILES_CONTROLLER_SYSEX_COMMAND_ADDRESS3   2
#define MILES_CONTROLLER_SYSEX_COMMAND_DATA       3
#define MILES_CONTROLLER_SYSEX_COMMAND_FINAL_DATA 4

#define MILES_CONTROLLER_XMIDI_RANGE_BEGIN 110
#define MILES_CONTROLLER_XMIDI_RANGE_END 120

//#define MILES_MT32_PATCHES_COUNT 128
//#define MILES_MT32_CUSTOMTIMBRE_COUNT 64

//#define MILES_MT32_PATCHDATA_COMMONPARAMETER_SIZE 14
//#define MILES_MT32_PATCHDATA_PARTIALPARAMETER_SIZE 58
//#define MILES_MT32_PATCHDATA_PARTIALPARAMETERS_COUNT 4
//#define MILES_MT32_PATCHDATA_TOTAL_SIZE (MILES_MT32_PATCHDATA_COMMONPARAMETER_SIZE + (MILES_MT32_PATCHDATA_PARTIALPARAMETER_SIZE * MILES_MT32_PATCHDATA_PARTIALPARAMETERS_COUNT))

// Some engines using Miles assume a source neutral
// volume of 256, so use this by default.
#define MILES_DEFAULT_SOURCE_NEUTRAL_VOLUME 256


                enum MilesVersion {
                    MILES_VERSION_2 = 2,
                    MILES_VERSION_3
                };

                /*struct MilesMT32InstrumentEntry {
                    uint8_t bankId;
                    uint8_t patchId;
                    uint8_t commonParameter[MILES_MT32_PATCHDATA_COMMONPARAMETER_SIZE];
                    uint8_t partialParameters[MILES_MT32_PATCHDATA_PARTIALPARAMETERS_COUNT][MILES_MT32_PATCHDATA_PARTIALPARAMETER_SIZE];
                };*/

                /**
                 * Abstract class containing the interface for loading
                 * the XMIDI timbres specified in the timbre chunks of
                 * an XMIDI file.
                 */
                //class MidiDriver_Miles_Xmidi_Timbres {
                //public:
                //    virtual ~MidiDriver_Miles_Xmidi_Timbres() { }

                //    /**
                //     * Processes the timbre chunk specified for a track
                //     * in an XMIDI file. This will load the necessary
                //     * timbres into the MIDI device using SysEx messages.
                //     *
                //     * This function will likely return before all SysEx
                //     * messages have been sent. Use the isReady method to
                //     * check if the driver has finished preparing for
                //     * playback. Playback should not be started before
                //     * this process has finished.
                //     */
                //    virtual void processXMIDITimbreChunk(const uint8_t* timbreListPtr, uint32_t timbreListSize) = 0;
                //};

                //class MidiDriver_Miles_Midi : public MidiDriver_MT32GM, public MidiDriver_Miles_Xmidi_Timbres {
                //public:
                //    MidiDriver_Miles_Midi(MusicType midiType, MilesMT32InstrumentEntry* instrumentTablePtr, uint16 instrumentTableCount);
                //    ~MidiDriver_Miles_Midi();

                //public:
                //    using MidiDriver_MT32GM::send;
                //    void send(int8 source, uint32_t b) override;

                //    /**
                //     * De-initialize a source. Call this after playing a track or sound effect using this source.
                //     * This will unlock and unprotect channels used by this source and stop any active notes
                //     * from this source.
                //     * Automatically executed when an End Of Track meta event is received.
                //     */
                //    void deinitSource(uint8 source) override;

                //    void stopAllNotes(bool stopSustainedNotes = false) override;

                //    uint32_t property(int prop, uint32_t param) override;

                //    void processXMIDITimbreChunk(const uint8_t* timbreListPtr, uint32_t timbreListSize) override;

                //protected:
                //    void initControlData() override;
                //    void initMidiDevice() override;
                //    void applySourceVolume(uint8 source) override;

                //private:
                //    void writeRhythmSetup(uint8_t note, uint8_t customTimbreId);
                //    void writePatchTimbre(uint8_t patchId, uint8_t timbreGroup, uint8_t timbreId, bool useSysExQueue = false);
                //    void writePatchByte(uint8_t patchId, uint8_t index, uint8_t patchValue);
                //    void writeToSystemArea(uint8_t index, uint8_t value);

                //    const MilesMT32InstrumentEntry* searchCustomInstrument(uint8_t patchBank, uint8_t patchId);
                //    int16 searchCustomTimbre(uint8_t patchBank, uint8_t patchId);

                //    void setupPatch(uint8_t patchBank, uint8_t patchId, bool useSysExQueue = false);
                //    int16 installCustomTimbre(uint8_t patchBank, uint8_t patchId);

                //private:
                //    /**
                //     * This stores the values of the MIDI controllers for
                //     * a MIDI channel. It is used to keep track of controller
                //     * values while a channel is locked, so they can be
                //     * restored when the channel is unlocked.
                //     */
                //    struct MilesMidiChannelControlData : MidiChannelControlData {
                //        // Custom timbre data
                //        uint8_t   currentPatchBank;

                //        bool   usingCustomTimbre;
                //        uint8_t   currentCustomTimbreId;

                //        MilesMidiChannelControlData() : currentPatchBank(0),
                //            usingCustomTimbre(false),
                //            currentCustomTimbreId(0) { }
                //    };

                //    struct MidiChannelEntry {
                //        // True if this channel is locked. A locked channel will
                //        // only accept MIDI messages from the source that locked it.
                //        bool   locked;
                //        // The channel in the MIDI data of the lock source that
                //        // is assigned to this locked output channel. This is a
                //        // reverse lookup for MidiSource::channelMap.
                //        // -1 if the channel is not locked.
                //        int8   lockDataChannel;
                //        // True if this channel is protected from locking.
                //        // The channel can still be locked, but unprotected
                //        // channels will be prioritized.
                //        bool   lockProtected;
                //        // The source that protected this channel from locking.
                //        // -1 if the channel is not protected.
                //        int8   protectedSource;

                //        // The number of notes currently active on the channel.
                //        uint8  activeNotes;

                //        // The MIDI controller values currently used by the channel.
                //        MilesMidiChannelControlData* currentData;
                //        // The MIDI controller values set by the sources which are
                //        // not currently using the channel because it is locked.
                //        // These values will be set on the channel when the channel
                //        // is unlocked.
                //        MilesMidiChannelControlData* unlockData;

                //        MidiChannelEntry() : locked(false),
                //            lockDataChannel(-1),
                //            lockProtected(false),
                //            protectedSource(-1),
                //            activeNotes(0),
                //            currentData(0),
                //            unlockData(0) { }
                //    };

                //    /**
                //     * Send out a control change MIDI message using the specified data.
                //     * @param controlData The new MIDI controller value will be set on this MidiChannelControlData
                //     * @param sendMessage True if the message should be sent out to the device
                //     */
                //    void controlChange(uint8_t outputChannel, uint8_t controllerNumber, uint8_t controllerValue, int8 source, MidiChannelControlData& controlData, bool channelLockedByOtherSource = false) override;
                //    bool addActiveNote(uint8 outputChannel, uint8 note, int8 source) override;
                //    bool removeActiveNote(uint8 outputChannel, uint8 note, int8 source) override;
                //    /**
                //     * Removes active notes from the active notes registration on the specified channel.
                //     * @param sustainedNotes True if only sustained notes should be removed; otherwise only regular active notes will be removed
                //     */
                //    void removeActiveNotes(uint8 outputChannel, bool sustainedNotes) override;
                //    /**
                //     * Find and lock an output channel and reserve it for the specified
                //     * source. The output channel will be mapped to the specified data
                //     * channel.
                //     */
                //    void lockChannel(uint8 source, uint8 dataChannel);
                //    /**
                //     * Find an output channel to lock. This will be based on the number
                //     * of active notes on the channels and whether the channel is
                //     * protected or not.
                //     * @param useProtectedChannels When true, protected channels are considered for locking
                //     * @returns The output channel to lock, or -1 if no channel is available
                //     */
                //    int8 findLockChannel(bool useProtectedChannels = false);
                //    /**
                //     * Unlock an output channel. This will stop all notes on the channel,
                //     * restore the controller values and make it available to other sources.
                //     */
                //    void unlockChannel(uint8 outputChannel);
                //    /**
                //     * Send a program change MIDI message using the specified data.
                //     * @param controlData The new program value will be set on this MidiChannelControlData
                //     * @param sendMessage True if the message should be sent out to the device
                //     */
                //    void programChange(uint8_t outputChannel, uint8_t patchId, int8 source, MidiChannelControlData& controlData, bool channelLockedByOtherSource = false) override;

                //    void stopNotesOnChannel(uint8 outputChannelNumber);

                //    struct MidiCustomTimbreEntry {
                //        bool   used;
                //        bool   protectionEnabled;
                //        uint8_t   currentPatchBank;
                //        uint8_t   currentPatchId;

                //        uint32_t lastUsedNoteCounter;

                //        MidiCustomTimbreEntry() : used(false),
                //            protectionEnabled(false),
                //            currentPatchBank(0),
                //            currentPatchId(0),
                //            lastUsedNoteCounter(0) {}
                //    };

                //    struct MilesMT32SysExQueueEntry {
                //        uint32_t targetAddress;
                //        uint8_t   dataPos;
                //        uint8_t   data[MILES_CONTROLLER_SYSEX_QUEUE_SIZE];

                //        MilesMT32SysExQueueEntry() : targetAddress(0),
                //            dataPos(0) {
                //            memset(data, 0, sizeof(data));
                //        }
                //    };

                //    // the version of Miles AIL/MSS to emulate
                //    MilesVersion _milesVersion;

                //    // stores information about all MIDI channels
                //    MidiChannelEntry _midiChannels[MIDI_CHANNEL_COUNT];

                //    // stores information about all custom timbres
                //    MidiCustomTimbreEntry _customTimbres[MILES_MT32_CUSTOMTIMBRE_COUNT];

                //    uint8_t _patchesBank[MILES_MT32_PATCHES_COUNT];

                //    // holds all instruments
                //    MilesMT32InstrumentEntry* _instrumentTablePtr;
                //    uint16                   _instrumentTableCount;

                //    uint32_t _noteCounter; // used to figure out, which timbres are outdated

                //    // Queues for Miles SysEx controllers
                //    MilesMT32SysExQueueEntry _milesSysExQueues[MILES_CONTROLLER_SYSEX_QUEUE_COUNT];
                //};

                //extern MidiDriver_Multisource* MidiDriver_Miles_AdLib_create(const Common::String& filenameAdLib, const Common::String& filenameOPL3, Common::SeekableReadStream* streamAdLib = nullptr, Common::SeekableReadStream* streamOPL3 = nullptr);
                //extern MidiDriver_Multisource* MidiDriver_Miles_AdLib_create(const std::string& filenameAdLib, const std::string& filenameOPL3, Common::SeekableReadStream* streamAdLib = nullptr, Common::SeekableReadStream* streamOPL3 = nullptr);
                //extern MidiDriver_Multisource* MidiDriver_Miles_AdLib_create();

                //extern MidiDriver_Miles_Midi* MidiDriver_Miles_MT32_create(const Common::String& instrumentDataFilename);

                //extern MidiDriver_Miles_Midi* MidiDriver_Miles_MIDI_create(MusicType midiType, const Common::String& instrumentDataFilename);
                /** @} */
            }
        }
    }
}
