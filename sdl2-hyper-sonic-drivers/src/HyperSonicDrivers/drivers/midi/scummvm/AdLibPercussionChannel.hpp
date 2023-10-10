#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibInstrument.h>


namespace HyperSonicDrivers::drivers::midi::scummvm
{
    /**
    * TODO: It should be derived from IMidiChannel as it has very little in common from AdLibChannel class
    **/
    class AdLibPercussionChannel : public AdLibChannel
    {
    public:
        AdLibPercussionChannel();
        ~AdLibPercussionChannel() override = default;

        uint8_t getNote(const uint8_t note) const noexcept;
        AdLibInstrument* getCustomInstrument(const uint8_t note) const noexcept;

        void setCustomInstr(const uint8_t* instr) noexcept override;

    private:
        std::array<uint8_t, 256> m_notes = { 0 };
        std::array<std::unique_ptr<AdLibInstrument>, 256> m_customInstruments;
    };
}
