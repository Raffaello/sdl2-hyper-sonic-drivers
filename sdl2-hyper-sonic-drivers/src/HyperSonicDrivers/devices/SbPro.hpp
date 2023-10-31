#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>

namespace HyperSonicDrivers::devices
{
    class SbPro : public Opl
    {
    public:
        SbPro(SbPro&) = delete;
        SbPro(SbPro&&) = delete;
        SbPro& operator=(SbPro&) = delete;

        explicit SbPro(
            const std::shared_ptr<audio::IMixer>& mixer,
            const hardware::opl::OplEmulator emulator = hardware::opl::OplEmulator::AUTO,
            const uint8_t volume = 255,
            const uint8_t pan = 0
        );

        ~SbPro() override = default;

        eDeviceName getName() const noexcept override;
    };
}
