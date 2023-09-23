#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/drivers/IMusicDriver.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>
#include <HyperSonicDrivers/hardware/IHardware.hpp>

namespace HyperSonicDrivers::devices
{
    /**
    * general interface for sound cards used by the drivers
    **/
    class IDevice
    {
    public:
        //IDevice(IDevice&) = delete;
        //IDevice(IDevice&&) = delete;
        //IDevice& operator=(IDevice&) = delete;

        IDevice(const std::shared_ptr<audio::IMixer>& mixer, const bool isOpl = false);
        virtual ~IDevice() = default;

        virtual bool init() noexcept = 0;
        inline bool isInit() const noexcept { return m_init; };
        virtual bool shutdown() noexcept = 0;

        //virtual void sendEvent(const audio::midi::MIDIEvent& e) const noexcept = 0;
        //virtual void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept = 0;
        //virtual void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept = 0;
        //virtual void pause() const noexcept = 0;
        //virtual void resume() const noexcept = 0;

        virtual std::optional<uint8_t> getChannelId() const noexcept = 0;

        inline bool isAcquired() const noexcept { return m_acquired; }
        inline bool isOwned(const  drivers::IMusicDriver* owner) const noexcept { return m_owner == owner; }

        // TODO: instead of acquire/release mechanism,
        //       consider to use a std_unique_ptr to pass to the drv /w a std::move
        //       or even simpler a enum of Device and the driver will create it....
        //       annoying for opl that requires the emulator to be passed in too. (so it can't be done, unless flat it down like:)
        //       MAME2, MAME3, DOS_BOX2, DOS_BOX_DUAL2, DOS_BOX3, NUKED, ....
        bool acquire(drivers::IMusicDriver* owner);
        bool release(const drivers::IMusicDriver* owner);

        // helpers methods
        void setVolume(const uint8_t volume);
        void setPan(const uint8_t pan);
        void setVolumePan(const uint8_t volume, const uint8_t pan);

        inline std::shared_ptr<audio::IMixer> getMixer() const noexcept { return m_mixer; };
        inline bool isOpl() const noexcept { return m_isOpl; };
        virtual hardware::IHardware* getHardware() const noexcept { return m_hardware; };
    protected:
        bool m_init = false;
        std::shared_ptr<audio::IMixer> m_mixer;
        //std::unique_ptr<drivers::midi::IMidiDriver> m_midiDriver;
        hardware::IHardware* m_hardware;
    private:
        bool m_isOpl = false;
        // TODO: remove the atomic when doing with callback instead of thread
        std::atomic<bool> m_acquired = false;
        std::atomic<drivers::IMusicDriver*> m_owner = nullptr;
    };

    template<class T, class To = devices::IDevice, typename... Args>
    std::shared_ptr<To> make_device(Args... args)
    {
        return std::dynamic_pointer_cast<To>(std::make_shared<T>(args...));
    }
}
