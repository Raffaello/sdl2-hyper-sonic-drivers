#include <HyperSonicDrivers/drivers/midi/opl/OplChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>
#include <memory>

namespace HyperSonicDrivers::drivers::midi::opl
{
    OplChannel::OplChannel(const uint8_t channel/*, std::vector<std::unique_ptr<OplVoice>>& voices*/) :
        IMidiChannel(channel)/*, m_oplVoices(voices)*/
    {
    }

    //void OplChannel::noteOff(const uint8_t note) noexcept
    //{
    //    /*for (auto it = m_voices.begin(); it!=m_voices.end();)
    //    {
    //        auto& v = dynamic_cast<OplVoice&>(*it);
    //        if (v.noteOff(note, sustain))
    //            m_voices.erase(it);
    //        else
    //            ++it;
    //    }*/
    //}

    //void OplChannel::noteOn(const uint8_t note, const uint8_t vol) noexcept
    //{
    //    
    //}
    //
    //void OplChannel::controller(const uint8_t ctrl, uint8_t value) noexcept
    //{
    //}
    //
    //void OplChannel::programChange(const uint8_t program) noexcept
    //{
    //}
    //
    //void OplChannel::pitchBend(const uint16_t bend) noexcept
    //{
    //}
    //
    //int OplChannel::allocateVoice(const uint8_t slot, const uint8_t channel, const uint8_t note, const uint8_t volume, const audio::opl::banks::Op2BankInstrument_t* instrument, const bool secondary) noexcept
    //{
    //    //OplVoice v()
    //    //return m_voices[slot]->allocate(this, ch, note, volume, instrument, secondary);
    //    return -1;
    //}
}
