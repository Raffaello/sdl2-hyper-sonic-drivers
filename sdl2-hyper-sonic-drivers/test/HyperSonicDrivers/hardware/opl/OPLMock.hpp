#pragma once

#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <cstdint>

namespace HyperSonicDrivers::hardware::opl
{
    class OPLMock : public OPL
    {
    public:
        OPLMock() : OPL(OplType::OPL2) {}
        bool init() override { _init = true; return true; }
        void reset() override {};
        void write(const uint32_t port, const uint16_t val) noexcept override {};
        uint8_t read(const uint32_t port) noexcept override { return 0; };
        void writeReg(const uint16_t r, const uint16_t v) noexcept override {};
        uint32_t setCallbackFrequency(int timerFrequency) override { return 1; }
        void startCallbacks(int timerFrequency) override {};
        void stopCallbacks() override {};
    };
}
