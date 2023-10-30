#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>


namespace HyperSonicDrivers::devices
{
    class Opl : public IDevice
    {
    protected:
        explicit Opl(
            const std::shared_ptr<audio::IMixer>& mixer,
            const hardware::opl::OplEmulator emulator,
            const hardware::opl::OplType type,
            const uint8_t volume,
            const uint8_t pan
        );

    public:
        bool init() noexcept override;
        bool shutdown() noexcept override;

        inline std::shared_ptr<hardware::opl::OPL> getOpl() const noexcept { return m_opl; };
        inline hardware::opl::OPL* getHardware() const noexcept override { return dynamic_cast<hardware::opl::OPL*>(IDevice::getHardware()); };
        inline hardware::opl::OplEmulator getEmulatorType() const noexcept { return m_emulator; };
    private:
        std::shared_ptr<hardware::opl::OPL> m_opl;
        const hardware::opl::OplEmulator m_emulator;
    };
}
