#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>
#include <HyperSonicDrivers/hardware/IHardware.hpp>
#include <HyperSonicDrivers/devices/types.hpp>

namespace HyperSonicDrivers
{
    namespace drivers
    {
        class IMusicDriver;
    }

    namespace devices
    {
        /**
        * general interface for sound cards used by the drivers
        * that is bound to a specific hardware
        **/
        class IDevice
        {
        public:
            IDevice(const std::shared_ptr<audio::IMixer>& mixer, const eDeviceType type);
            virtual ~IDevice() = default;

            virtual bool init() noexcept = 0;
            virtual bool shutdown() noexcept = 0;
            inline bool isInit() const noexcept { return m_init; };

            std::optional<uint8_t> getChannelId() const noexcept { return m_hardware->getChannelId(); };

            inline bool isAcquired() const noexcept { return m_acquired; }
            inline bool isOwned(const  drivers::IMusicDriver* owner) const noexcept { return m_owner == owner; }

            bool acquire(drivers::IMusicDriver* owner);
            bool release(const drivers::IMusicDriver* owner);

            // helpers methods
            void setVolume(const uint8_t volume);
            void setPan(const uint8_t pan);
            void setVolumePan(const uint8_t volume, const uint8_t pan);

            inline std::shared_ptr<audio::IMixer> getMixer() const noexcept { return m_mixer; };
            virtual hardware::IHardware* getHardware() const noexcept { return m_hardware; };
            virtual eDeviceName getName() const noexcept = 0;
            const eDeviceType type;
        protected:
            bool m_init = false;
            std::shared_ptr<audio::IMixer> m_mixer;
            hardware::IHardware* m_hardware = nullptr;
        private:
            // TODO: remove the atomic when removing the thread in MIDDrv
            std::atomic<bool> m_acquired = false;
            std::atomic<drivers::IMusicDriver*> m_owner = nullptr;
        };

        template<class T, class To = devices::IDevice, typename... Args>
        std::shared_ptr<To> make_device(Args... args)
        {
            return std::dynamic_pointer_cast<To>(std::make_shared<T>(args...));
        }
    }
}
