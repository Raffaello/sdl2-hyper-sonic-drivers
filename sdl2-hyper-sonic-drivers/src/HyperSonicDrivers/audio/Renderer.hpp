#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>

namespace HyperSonicDrivers::audio
{
    class Renderer
    {
    public:
        explicit Renderer(const size_t buffer_size);
        virtual ~Renderer() = default;

        void setOutputFile(const std::filesystem::path& path);
        void releaseOutputFile() noexcept;

        void renderBuffer(IAudioStream* stream);
        inline void renderBuffer(const std::shared_ptr<hardware::opl::OPL>& opl) { renderBuffer(opl->getAudioStream().get()); };
        inline void renderBuffer(const std::shared_ptr<devices::Opl>& opl) { renderBuffer(opl->getHardware()->getAudioStream().get()); };

    private:
        const size_t m_buf_size;
        std::unique_ptr<files::WAVFile> m_out;
        std::vector<int16_t> m_buf;
    };
}
