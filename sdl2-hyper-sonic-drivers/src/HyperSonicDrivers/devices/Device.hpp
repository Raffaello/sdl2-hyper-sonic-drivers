#pragma once

namespace HyperSonicDrivers::devices
{
    class Device
    {
    public:
        Device(Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(Device&) = delete;

        Device() = default;
        virtual ~Device() = default;
    protected:

    };
}
