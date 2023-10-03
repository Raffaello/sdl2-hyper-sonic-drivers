#pragma once

#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibInstrument.h>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    struct AdLibVoice;
    class MidiDriver_ADLIB;

    class AdLibPart : public MidiChannel
    {
        friend class MidiDriver_ADLIB;

    protected:
        //	AdLibPart *_prev, *_next;
        AdLibVoice* _voice = nullptr;
        uint8_t _pitchBendFactor = 2;
        //int8_t _transposeEff;
        int8_t _detuneEff = 0;
        uint8_t _priEff = 127;
        //uint8_t pan = 64;
        AdLibInstrument _partInstr;
        AdLibInstrument _partInstrSecondary;

        MidiDriver_ADLIB* _owner = nullptr;
        bool _allocated = false;

        void init(MidiDriver_ADLIB* owner);
        void allocate(); // TODO: this is more relative to AdLibVoice instead, Midi channel are 16, eventually are in use

    public:
        AdLibPart(const uint8_t channel);

        inline const AdLibInstrument* getInstr() const noexcept { return &_partInstr; };
        inline const AdLibInstrument* getInstrSecondary() const noexcept { return &_partInstrSecondary; };
        void setInstr(const bool isOpl3) noexcept;


        MidiDriver* device() override;
        void release() override;

        void send(uint32_t b) override;

        // Regular messages
        //void noteOff(uint8_t note) override;
        //void noteOn(uint8_t note, uint8_t velocity) override;
        //void programChange(uint8_t program) override;
        //void pitchBend(int16_t bend) override;

        // Control Change messages
        void controlChange(uint8_t control, uint8_t value) override;
        void modulationWheel(uint8_t value) override;
        void setVolume(uint8_t value) override;
        void panPosition(uint8_t value) override;
        void pitchBendFactor(uint8_t value) override;
        void detune(uint8_t value) override;
        void priority(uint8_t value) override;
        void setSustain(const uint8_t value) override;
        void effectLevel(uint8_t value) override { return; } // Not supported
        void chorusLevel(uint8_t value) override { return; } // Not supported
        void allNotesOff() override;

        // SysEx messages
        void sysEx_customInstrument(uint32_t type, const uint8_t* instr) override;
    };
}
