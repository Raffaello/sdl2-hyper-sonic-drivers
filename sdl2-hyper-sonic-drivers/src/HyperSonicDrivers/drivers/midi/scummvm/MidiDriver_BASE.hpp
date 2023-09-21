#pragma once

#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    /**
     * TODO: Document this, give it a better name.
     */
    class MidiDriver_BASE : public IMidiDriver
    {
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

        MidiDriver_BASE() = default;
        virtual ~MidiDriver_BASE() = default;

        using IMidiDriver::send;

        /**
         * Output a packed midi command to the midi stream.
         * The 'lowest' uint8_t (i.e. b & 0xFF) is the status
         * code, then come (if used) the first and second
         * opcode.
         */
        //virtual void send(uint32_t b) = 0;

        /**
         * Send a MIDI command from a specific source. If the MIDI driver
         * does not support multiple sources, the source parameter is
         * ignored.
         */
        //virtual void send(int8_t channel, uint32_t b) { send(b); }

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
    };
}
