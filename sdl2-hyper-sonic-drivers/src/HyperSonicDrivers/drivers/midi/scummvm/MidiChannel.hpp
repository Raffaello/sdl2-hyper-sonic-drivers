#pragma once

#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    class MidiChannel
    {
    public:
        virtual ~MidiChannel() = default;

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
}
