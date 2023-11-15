#include <HyperSonicDrivers/audio/sdl2/Renderer.hpp>
#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <algorithm>
#include <ranges>


namespace HyperSonicDrivers::audio::sdl2
{
    Renderer::Renderer(const uint32_t freq, const uint16_t buffer_size, const uint8_t max_channels)
    {
        m_mixer = make_mixer<Mixer>(max_channels, freq, buffer_size);
    }

    void Renderer::openOutputFile(const std::filesystem::path& path)
    {
        m_out = std::make_unique<files::WAVFile>(path.string(), audio::mixer::eChannelGroup::Unknown, false);
        m_buf.resize(0);
    }

    void Renderer::closeOutputFile() noexcept
    {
        m_out.reset();
    }

    void Renderer::renderBuffer(IAudioStream* stream)
    {
        if (m_buf.empty())
        {
            m_out->save_prepare(stream->getRate(), stream->isStereo());
            m_buf.resize(m_mixer->buffer_size);
        }

        const size_t read = stream->readBuffer(m_buf.data(), m_buf.size());
        m_out->save_streaming(m_buf.data(), read);
    }

    void Renderer::renderFlush(IAudioStream* stream)
    {
        // safety check
        if (m_buf.empty())
        {
            m_out->save_prepare(stream->getRate(), stream->isStereo());
            m_buf.resize(m_mixer->buffer_size);
        }

        while (true)
        {
            const size_t read = stream->readBuffer(m_buf.data(), m_buf.size());
            const bool silenced = std::ranges::all_of(m_buf, [](const auto i) { return i == 0; });
            if (silenced)
                return;

            m_out->save_streaming(m_buf.data(), read);
        }
    }

    void Renderer::renderBuffer(IAudioStream* stream, drivers::IAudioDriver& drv, const int track)
    {
        drv.play(track);
        while (drv.isPlaying())
            renderBuffer(stream);

        renderFlush(stream);
        closeOutputFile();
    }
}
