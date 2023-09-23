#pragma once

#include <memory>
#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::devices
{
    class OplDeviceMock : public Opl
    {
    public:
        explicit OplDeviceMock(const hardware::opl::OplType type,
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::IMixer>& mixer)
            : Opl(mixer, emuType, type, 255, 0)
        {
        }
        ~OplDeviceMock() override = default;
    };
}
