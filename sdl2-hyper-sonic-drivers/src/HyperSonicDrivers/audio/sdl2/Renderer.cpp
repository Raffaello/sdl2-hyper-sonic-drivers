#include <HyperSonicDrivers/audio/sdl2/Renderer.hpp>
#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>


namespace HyperSonicDrivers::audio::sdl2
{
    Renderer::Renderer(const uint32_t freq, const uint16_t buffer_size)
    {
        m_mixer = make_mixer<Mixer>(1, freq, buffer_size);
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
            m_buf.resize(m_mixer->getBufferSize());
        }

        const size_t read = stream->readBuffer(m_buf.data(), m_buf.size());
        m_out->save_streaming(m_buf.data(), read);
    }
}
