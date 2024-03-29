#pragma once

#include <memory>
#include <HyperSonicDrivers/devices/IDevice.hpp>

namespace HyperSonicDrivers::drivers
{
    /**
    * Common Interface for music/sfx (audio) drivers: MIDI, ADL, XMI, MUS...
    * TODO/Rename: if not merged with IMidiDriver, this can be renamed as IAudioPlayer
    *              related to playing those files rather then using a driver,
    *              ADLDriver at the moment is both
    * 
    **/
    class IAudioDriver
    {
    public:
        IAudioDriver(IAudioDriver&) = delete;
        IAudioDriver(IAudioDriver&&) = delete;
        IAudioDriver& operator=(IAudioDriver&) = delete;

        explicit IAudioDriver(const std::shared_ptr<devices::IDevice>& device);
        virtual ~IAudioDriver() = default;

        virtual void play(const uint8_t track) noexcept = 0;
        virtual void stop() noexcept = 0;

        // TODO: it might not be required
        //virtual void pause() = 0;
        // TODO: it might not be required
        //virtual void resume() = 0;

        virtual bool isPlaying() const noexcept = 0;

        // TODO: it might not be required
        //virtual bool isPaused() const noexcept = 0;

        inline std::shared_ptr<devices::IDevice> getDevice() const noexcept { return m_device; };
    protected:
        // this is used to "lock" the device to a specific driver output and passed to IMidiDriver
        std::shared_ptr<devices::IDevice> m_device;
    };
}
