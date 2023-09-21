#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/IEmulatedHardware.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>

namespace HyperSonicDrivers::audio
{
    class IRenderer;

    namespace streams
    {
        class OplStream;
    }
}

namespace HyperSonicDrivers::hardware::opl
{
    /**
     * An OPL that represents an emulated OPL.
     */
    class EmulatedOPL : public OPL, public IEmulatedHardware
    {
        friend audio::IRenderer;
        friend audio::streams::OplStream;

    public:
        EmulatedOPL(const OplType type, const std::shared_ptr<audio::IMixer>& mixer);
        ~EmulatedOPL() override = default;

    protected:
        void startCallbacks(
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan,
            const int timerFrequency) override;

        std::shared_ptr<audio::IAudioStream> getAudioStream() const noexcept override;

    private:
        std::shared_ptr<audio::IAudioStream> m_stream;
    };
}
