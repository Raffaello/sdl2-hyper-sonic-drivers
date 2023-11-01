#pragma once

#include <filesystem>
#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>

namespace HyperSonicDrivers::audio
{
    class IRenderer
    {
    public:
        IRenderer() = default;
        virtual ~IRenderer() = default;

        virtual void openOutputFile(const std::filesystem::path& path) = 0;
        virtual void closeOutputFile() noexcept = 0;

        inline std::shared_ptr<IMixer> getMixer() const noexcept { return m_mixer; };

        virtual void renderBuffer(IAudioStream* stream) = 0;
        inline void renderBuffer(const std::shared_ptr<devices::IDevice>& device) { renderBuffer(device->getHardware()->getAudioStream().get()); };

    protected:
        std::shared_ptr<IMixer> m_mixer;
    };
}
