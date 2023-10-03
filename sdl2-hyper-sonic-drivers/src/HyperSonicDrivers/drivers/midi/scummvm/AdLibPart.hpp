#pragma once

#include <cstdint>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibInstrument.h>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    struct AdLibVoice;

    class AdLibPart : public IMidiChannel
    {
    public:
        explicit AdLibPart(const uint8_t channel);

        inline const AdLibInstrument* getInstr() const noexcept { return &_partInstr; };
        inline const AdLibInstrument* getInstrSecondary() const noexcept { return &_partInstrSecondary; };
        void setInstr(const bool isOpl3) noexcept;
        void setCustomInstr(const AdLibInstrument* instr) noexcept;

        AdLibVoice* voice = nullptr;
        uint8_t pitchBendFactor = 2;
        int8_t detuneEff = 0;
        uint8_t priEff = 127;
        //int8_t _transposeEff;

    protected:
        AdLibInstrument _partInstr;
        AdLibInstrument _partInstrSecondary;
    };
}
