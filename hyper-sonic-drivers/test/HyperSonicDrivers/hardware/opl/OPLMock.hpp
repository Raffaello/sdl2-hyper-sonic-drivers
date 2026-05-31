#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    class OPLMock : public OPL
    {
    public:
        OPLMock() : OPL(std::make_shared<audio::IMixerMock>(), OplType::OPL2) {};
        bool init() override { m_init = true; return true; };
        void reset() override {};
        void write(const uint32_t port, const uint16_t val) noexcept override {};
        uint8_t read(const uint32_t port) noexcept override { return 0; };
        void writeReg(const uint16_t r, const uint16_t v) noexcept override {};
        uint32_t setCallbackFrequency(int timerFrequency) override { return 1; };

        /*void startCallbacks(
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan,
            const int timerFrequency
        ) override {};*/

        void generateSamples(int16_t* buffer, const size_t length) noexcept {}
    };
}
