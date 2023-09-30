#pragma once

#include <cstdint>
//#include <vector>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    class OplChannel : public IMidiChannel
    {
    public:
        OplChannel(const uint8_t channel/*, std::vector<std::unique_ptr<OplVoice>>& voices*/);

    protected:
        //void noteOff(const uint8_t note) noexcept override;
        //void noteOn(const uint8_t note, const uint8_t vol) noexcept override;
        //void controller(const uint8_t ctrl, uint8_t value) noexcept override;
        //void programChange(const uint8_t program) noexcept override;
        //void pitchBend(const uint16_t bend) noexcept override;

    private:
        //std::vector<std::unique_ptr<OplVoice>>& m_oplVoices;
        //audio::opl::banks::Op2BankInstrument_t* m_instrPtr = nullptr;

        //int allocateVoice(const uint8_t slot, const uint8_t channel,
        //    const uint8_t note, const uint8_t volume,
        //    const audio::opl::banks::Op2BankInstrument_t* instrument,
        //    const bool secondary) noexcept;
    };
}
