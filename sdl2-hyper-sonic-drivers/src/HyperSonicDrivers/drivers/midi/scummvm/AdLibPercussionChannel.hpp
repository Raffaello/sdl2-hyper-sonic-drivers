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
    public:
        AdLibPercussionChannel();
        ~AdLibPercussionChannel() override = default;

        uint8_t getNote(const uint8_t note) const noexcept;
        AdLibInstrument* getInstrument(const uint8_t note) const noexcept;

        void setNote(const uint8_t note, const uint8_t value) noexcept;
        void setCustomInstr(const uint8_t* instr) noexcept;

    private:
        std::array<uint8_t, 256> m_notes = { 0 };
        std::array<std::unique_ptr<AdLibInstrument>, 256> m_customInstruments;
    };
}
