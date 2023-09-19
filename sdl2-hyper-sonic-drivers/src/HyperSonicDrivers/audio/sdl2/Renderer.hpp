#pragma once

#include <HyperSonicDrivers/audio/IRenderer.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/opl/EmulatedOPL.hpp>
#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <vector>
#include <memory>
#include <filesystem>

namespace HyperSonicDrivers::audio::sdl2
{
    class Renderer : public IRenderer
    {
    public:
        Renderer(const uint32_t freq, const uint16_t buffer_size);
        ~Renderer() override = default;

        void setOutputFile(const std::filesystem::path& path) override;
        void releaseOutputFile() noexcept override;

        void renderBuffer(IAudioStream* stream) override;
        using IRenderer::renderBuffer;
    private:
        std::unique_ptr<files::WAVFile> m_out;
        std::vector<int16_t> m_buf;
    };
}
