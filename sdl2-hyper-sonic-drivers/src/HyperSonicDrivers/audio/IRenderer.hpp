#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/drivers/IAudioDriver.hpp>
#include <HyperSonicDrivers/files/WAVFile.hpp>

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
        virtual bool renderFlush(IAudioStream* stream) = 0;
        inline bool renderFlush(const std::shared_ptr<devices::IDevice>& device) { return renderFlush(device->getHardware()->getAudioStream().get()); };

        virtual bool renderBuffer(IAudioStream* stream, drivers::IAudioDriver& drv, const int track) = 0;
        inline bool renderBuffer(const std::shared_ptr<devices::IDevice>& device, drivers::IAudioDriver& drv, const int track) { return renderBuffer(device->getHardware()->getAudioStream().get(), drv, track); };
    protected:
        std::shared_ptr<IMixer> m_mixer;
        std::unique_ptr<files::WAVFile> m_out;
        std::vector<int16_t> m_buf;
    };
}
