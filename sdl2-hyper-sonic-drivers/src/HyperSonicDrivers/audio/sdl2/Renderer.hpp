#pragma once

#include <cstdint>
#include <filesystem>
#include <HyperSonicDrivers/audio/IRenderer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>

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
    };
}
