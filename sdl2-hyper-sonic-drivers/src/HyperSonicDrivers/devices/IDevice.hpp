#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/devices/IMidiDevice.hpp>

namespace HyperSonicDrivers::devices
{
    /**
    * general interface for sound cards used by the drivers
    **/
    class IDevice
    {
    protected:
        IDevice(IDevice&) = delete;
        IDevice(IDevice&&) = delete;
        IDevice& operator=(IDevice&) = delete;

        IDevice(const std::shared_ptr<audio::IMixer>& mixer);
        virtual ~IDevice() = default;

        virtual std::optional<uint8_t> getChannelId() const noexcept = 0;

        // helpers methods
        void setVolume(const uint8_t volume);
        void setPan(const uint8_t pan);
        void setVolumePan(const uint8_t volume, const uint8_t pan);

        inline std::shared_ptr<audio::IMixer> getMixer() const noexcept { return m_mixer; };
    protected:
        std::shared_ptr<audio::IMixer> m_mixer;
    };
}
