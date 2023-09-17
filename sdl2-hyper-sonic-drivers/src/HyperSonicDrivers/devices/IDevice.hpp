#pragma once

#include <cstdint>
#include <memory>

namespace HyperSonicDrivers::devices
{
    /**
    * general interface for sound cards.
    * NOTE:
    * At the moment only to aim to play OPL/Adlib ADLFiles as a lot of rewriting is required to generalize it
    **/
    class IDevice
    {
    public:
        IDevice(IDevice&) = delete;
        IDevice(IDevice&&) = delete;
        IDevice& operator=(IDevice&) = delete;

    protected:
        IDevice(const std::shared_ptr<audio::IMixer>& mixer,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) :
            m_mixer(mixer), m_group(group), m_volume(volume), m_pan(pan)
        {};

        virtual ~IDevice() = default;

    protected:
        std::shared_ptr<audio::IMixer> m_mixer;
        const audio::mixer::eChannelGroup m_group;
        const uint8_t m_volume;
        const uint8_t m_pan;
    };
}
