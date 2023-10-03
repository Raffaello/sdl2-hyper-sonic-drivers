#include <HyperSonicDrivers/drivers/midi/IMidiChannelVoice.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>
#include <cmath>

namespace HyperSonicDrivers::drivers::midi
{
    uint8_t IMidiChannelVoice::getChannelNum() const noexcept
    {
        return m_channel->channel;
    }

    void IMidiChannelVoice::setVolumes(const uint8_t volume) noexcept
    {
        m_volume = volume;
        m_real_volume = calcVolume_();
    }

    uint8_t IMidiChannelVoice::calcVolume_() const noexcept
    {
        return  std::min<uint8_t>(static_cast<uint8_t>(m_volume * m_channel->volume / 127), 127);
    }
}
