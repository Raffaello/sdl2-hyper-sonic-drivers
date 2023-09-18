#pragma once

#include <HyperSonicDrivers/audio/IRenderer.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/opl/EmulatedOPL.hpp>
#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>

namespace HyperSonicDrivers::audio::sdl2
{
    class Renderer : public IRenderer
    {
    public:
        Renderer(const uint32_t freq, const uint16_t buffer_size);
        ~Renderer() = default;

        void setOutputFile(const std::filesystem::path& path) override;
        void releaseOutputFile() noexcept override;

        inline std::shared_ptr<IMixer> getMixer() const noexcept override { return m_mixer; };

        void renderBuffer(IAudioStream* stream) override;
        using IRenderer::renderBuffer;

        //void render(const std::filesystem::path& outfile);
    private:
        std::shared_ptr<IMixer> m_mixer;
        //std::ofstream m_out;
        std::unique_ptr<files::WAVFile> m_out;
        std::vector<int16_t> m_buf;
        //size_t callback(uint8_t* samples, unsigned int len);
        //static void sdlCallback(void* this_, uint8_t* samples, int len);
    };
}
