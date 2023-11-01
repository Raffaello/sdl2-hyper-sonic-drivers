#pragma once

#include <HyperSonicDrivers/audio/IRenderer.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <cstdint>
#include <vector>
#include <memory>
#include <filesystem>

namespace HyperSonicDrivers::audio::sdl2
{
    class Renderer : public IRenderer
    {
    public:
        Renderer(const uint32_t freq, const uint16_t buffer_size, const uint8_t max_channels = 1);
        ~Renderer() override = default;

        void openOutputFile(const std::filesystem::path& path) override;
        void closeOutputFile() noexcept override;

        void renderBuffer(IAudioStream* stream) override;
        using IRenderer::renderBuffer;
    private:
        std::unique_ptr<files::WAVFile> m_out;
        std::vector<int16_t> m_buf;
    };
}
