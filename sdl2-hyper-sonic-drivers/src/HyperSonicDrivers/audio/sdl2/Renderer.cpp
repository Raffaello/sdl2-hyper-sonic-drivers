#include <HyperSonicDrivers/audio/sdl2/Renderer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <stdexcept>
#include <fstream>
#include <cassert>

namespace HyperSonicDrivers::audio::sdl2
{
    Renderer::Renderer(const uint32_t freq, const uint16_t buffer_size)
    {
        m_mixer = std::dynamic_pointer_cast<Mixer>(make_mixer<sdl2::Mixer>(1, freq, buffer_size));
        if (!m_mixer->init_(Renderer::sdlCallback, this))
        {
            utils::throwLogC<std::runtime_error>("can't init mixer");
        }
    }

    void HyperSonicDrivers::audio::sdl2::Renderer::setOutputFile(const std::filesystem::path& path)
    {
        if (m_out.is_open())
            m_out.close();

        m_out.open(path, std::ios::binary | std::ios::out);
        if (!m_out.good() || !m_out.is_open())
        {
            throw std::runtime_error("???");
        }
    }

    size_t Renderer::callback(uint8_t* samples, unsigned int len)
    {
        const auto res = m_mixer->callback(samples, len);
        m_out.write(reinterpret_cast<const char*>(samples), len);
        return res;
    }

    void Renderer::sdlCallback(void* userdata, uint8_t* stream, int len)
    {
        Renderer* r = reinterpret_cast<Renderer*>(userdata);
        assert(r != nullptr);
        r->callback(stream, len);
    }
}
