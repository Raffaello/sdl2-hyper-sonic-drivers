#pragma once

#include <filesystem>
#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/devices/Opl.hpp>

namespace HyperSonicDrivers::audio
{
    class IRenderer
    {
    public:
        IRenderer() = default;
        virtual ~IRenderer() = default;

        virtual void setOutputFile(const std::filesystem::path& path) = 0;
        virtual void releaseOutputFile() noexcept = 0;

        inline std::shared_ptr<IMixer> getMixer() const noexcept { return m_mixer; };

        virtual void renderBuffer(IAudioStream* stream) = 0;
        inline void renderBuffer(const std::shared_ptr<hardware::opl::OPL>& opl) { renderBuffer(opl->getAudioStream().get()); };
        inline void renderBuffer(const std::shared_ptr<devices::Opl>& opl) { renderBuffer(opl->getHardware()->getAudioStream().get()); };

    protected:
        std::shared_ptr<IMixer> m_mixer;
    };
}
