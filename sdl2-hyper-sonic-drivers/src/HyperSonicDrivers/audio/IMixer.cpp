#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/consts.hpp>
#include <algorithm>


namespace HyperSonicDrivers::audio
{
    IMixer::IMixer(const uint8_t max_channels,
        const uint32_t freq, const uint16_t buffer_size//,
        /*const uint8_t bitsDepth*/) :
        max_channels(max_channels),
        m_sampleRate(freq), m_samples(buffer_size),
        //m_bitsDepth(bitsDepth)
        m_bitsDepth(16)
    {
    }

    uint8_t IMixer::getChannelGroupVolume(const mixer::eChannelGroup group) const noexcept
    {
        return m_group_settings[group2i(group)].volume;
    }

    void IMixer::setChannelGroupVolume(const mixer::eChannelGroup group, const uint8_t volume) noexcept
    {
        m_group_settings.at(group2i(group)).volume = volume;
    }
}
