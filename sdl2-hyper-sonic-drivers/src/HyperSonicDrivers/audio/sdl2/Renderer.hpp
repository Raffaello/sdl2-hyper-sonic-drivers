#pragma once

#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>

namespace HyperSonicDrivers::audio::sdl2
{
    class Renderer
    {
    public:
        Renderer(const uint32_t freq, const uint16_t buffer_size);
        ~Renderer() = default;

        void setOutputFile(const std::filesystem::path& path);
        inline std::shared_ptr<Mixer> getMixer() const noexcept { return m_mixer; };
        
        void renderBuffer(IAudioStream* stream);
    private:
        std::shared_ptr<Mixer> m_mixer;
        std::ofstream m_out;

        //size_t callback(uint8_t* samples, unsigned int len);
        static void sdlCallback(void* this_, uint8_t* samples, int len);
    };
}
