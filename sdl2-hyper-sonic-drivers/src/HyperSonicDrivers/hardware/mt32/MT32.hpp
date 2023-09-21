#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>
#include <optional>
#include <mt32emu/c_interface/cpp_interface.h>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/IHardware.hpp>

namespace HyperSonicDrivers::devices::midi
{
    class MidiMT32;
}

namespace HyperSonicDrivers::audio
{
    class IRenderer;

    namespace streams
    {
        class MT32Stream;
    }
}

namespace HyperSonicDrivers::hardware::mt32
{
    constexpr int mt32_frequency_internal = 32000; // 32 KHz
    constexpr int mt32_frequency = 250;

    class MT32 : public IHardware
    {
        friend devices::midi::MidiMT32;
        friend audio::streams::MT32Stream;

    public:
        MT32(const std::filesystem::path& control_rom, const std::filesystem::path& pcm_rom,
            const std::shared_ptr<audio::IMixer>& mixer
        );
        virtual ~MT32();

        inline bool isStereo() const noexcept override { return true; };
        bool init() override;
        void reset() override;

        void start(
            const std::shared_ptr<TimerCallBack>& callback,
            const audio::mixer::eChannelGroup group = audio::mixer::eChannelGroup::Music,
            const uint8_t volume = 255,
            const uint8_t pan = 0,
            const int timerFrequency = mt32_frequency) override;

    protected:
        void startCallbacks(
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan,
            const int timerFrequency
        ) override;

        void generateSamples(int16_t* buffer, const size_t length) noexcept override;

    private:
        MT32Emu::Service m_service;
    };
}
