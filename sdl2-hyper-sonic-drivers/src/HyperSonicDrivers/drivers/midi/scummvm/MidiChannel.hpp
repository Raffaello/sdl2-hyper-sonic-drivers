#pragma once

#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    // TODO: remove this class
    class MidiChannel : public IMidiChannel
    {
    public:
        MidiChannel(const uint8_t channel) : IMidiChannel(channel) {};
        virtual ~MidiChannel() = default;

        virtual MidiDriver* device() = 0;
        virtual void release() = 0;

        virtual void send(uint32_t b) = 0; // 4-bit channel portion is ignored

        // Regular messages
        //virtual void noteOff(uint8_t note) = 0;
        //virtual void noteOn(uint8_t note, uint8_t velocity) = 0;
        //virtual void programChange(uint8_t program) = 0;
        //virtual void pitchBend(int16_t bend) = 0; // -0x2000 to +0x1FFF

        // Control Change messages
        //virtual void controlChange(uint8_t control, uint8_t value) = 0;
        // TODO: remove the static_cast and pass the original type instead
        //virtual void modulationWheel(uint8_t value) { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::MODULATION_WHEEL), value); }
        //virtual void setVolume(uint8_t value) { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::CHANNEL_VOLUME), value); }
        //virtual void panPosition(uint8_t value) { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::PAN), value); }
        //virtual void pitchBendFactor(uint8_t value) = 0;
        //virtual void transpose(int8_t value) {}
        //virtual void detune(uint8_t value) { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::GENERAL_PURPOSE_CONTROLLER_2), value); }
        //virtual void priority(uint8_t value) { }
        //virtual void setSustain(const uint8_t value) { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::SUSTAIN), value); }
        //virtual void effectLevel(uint8_t value) { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::REVERB), value); }
        //virtual void chorusLevel(uint8_t value) { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::CHORUS), value); }
        //virtual void allNotesOff() { controlChange(static_cast<uint8_t>(audio::midi::MIDI_EVENT_CONTROLLER_TYPES::ALL_NOTES_OFF), 0); }

        // SysEx messages
        virtual void sysEx_customInstrument(uint32_t type, const uint8_t* instr) = 0;
    };
}
