#pragma once
#include <hardware/opl/OPL.hpp>
#include <hardware/opl/OplType.hpp>
#include <audio/scummvm/SoundHandle.hpp>
#include <cstdint>

namespace hardware::opl
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
        void setCallbackFrequency(int timerFrequency) override {}
        void startCallbacks(int timerFrequency) override {};
        void stopCallbacks() override {};
        std::shared_ptr<audio::scummvm::SoundHandle> getSoundHandle() const noexcept override { return nullptr; }
    };
}
