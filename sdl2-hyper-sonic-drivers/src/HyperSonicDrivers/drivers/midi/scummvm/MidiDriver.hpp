#pragma once

#include <string>
#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_BASE.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    class MidiChannel;

    /**
     * Abstract MIDI Driver Class
     *
     * @todo Rename MidiDriver to MusicDriver
     */
    class MidiDriver : public MidiDriver_BASE
    {
    public:
        ~MidiDriver() override = default;

        static const uint8_t _mt32ToGm[128];
        static const uint8_t _gmToMt32[128];

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
        virtual uint32_t property(int prop, uint32_t param) = 0;

        /** Retrieve a string representation of an error code. */
        //static const char* getErrorName(int error_code);

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
        //virtual void setTimerCallback(void* timer_param, Common::TimerManager::TimerProc timer_proc) = 0;

        /** The time in microseconds between invocations of the timer callback. */
        virtual uint32_t getBaseTempo() = 0;

        // Channel allocation functions
        virtual MidiChannel* allocateChannel() = 0;
        virtual MidiChannel* getPercussionChannel() = 0;

        // Allow an engine to supply its own soundFont data. This stream will be destroyed after use.
        //virtual void setEngineSoundFont(Common::SeekableReadStream* soundFontData) { }

        // Does this driver accept soundFont data?
        //virtual bool acceptsSoundFontData() { return false; }
    };
}
