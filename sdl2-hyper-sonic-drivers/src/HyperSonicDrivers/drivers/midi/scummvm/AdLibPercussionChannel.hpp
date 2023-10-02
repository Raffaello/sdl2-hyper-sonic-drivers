#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibPart.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibInstrument.h>


namespace HyperSonicDrivers::drivers::midi::scummvm
{
    // FYI (Jamieson630)
    // It is assumed that any invocation to AdLibPercussionChannel
    // will be done through the MidiChannel base class as opposed to the
    // AdLibPart base class. If this were NOT the case, all the functions
    // listed below would need to be virtual in AdLibPart as well as MidiChannel.
    class AdLibPercussionChannel : public AdLibPart
    {
        friend class MidiDriver_ADLIB;

    protected:
        void init(MidiDriver_ADLIB* owner);

    public:
        AdLibPercussionChannel() : AdLibPart(audio::midi::MIDI_PERCUSSION_CHANNEL) {};
        ~AdLibPercussionChannel() override = default;

        void noteOff(uint8_t note) override;
        void noteOn(uint8_t note, uint8_t velocity) override;
        void programChange(uint8_t program) override { }

        // Control Change messages
        void modulationWheel(uint8_t value) override { }
        void pitchBendFactor(uint8_t value) override { }
        void detune(uint8_t value) override { }
        void priority(uint8_t value) override { }
        void sustain(bool value) override { }

        // SysEx messages
        void sysEx_customInstrument(uint32_t type, const uint8_t* instr) override;

    private:
        std::array<uint8_t, 256> _notes = { 0 };
        std::array<std::unique_ptr<AdLibInstrument>, 256> _customInstruments;
    };
}
