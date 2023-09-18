#pragma once

#include <filesystem>
#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/opl/EmulatedOPL.hpp>

namespace HyperSonicDrivers::audio
{
    class IRenderer
    {
    public:
        IRenderer() = default;
        virtual ~IRenderer() = default;

        virtual void setOutputFile(const std::filesystem::path& path) = 0;
        virtual void releaseOutputFile() noexcept = 0;
        virtual inline std::shared_ptr<IMixer> getMixer() const noexcept = 0;

        virtual void renderBuffer(IAudioStream* stream) = 0;
        void renderBuffer(std::shared_ptr<hardware::opl::EmulatedOPL>& opl) { renderBuffer(opl->m_stream.get()); };
    };
}
