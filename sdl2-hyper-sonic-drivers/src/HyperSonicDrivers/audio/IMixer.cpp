#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/consts.hpp>
#include <algorithm>


namespace HyperSonicDrivers::audio
{
    IMixer::IMixer(const uint8_t max_channels) :
        max_channels(max_channels)
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
