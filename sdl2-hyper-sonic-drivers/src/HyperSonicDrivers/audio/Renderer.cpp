#include <HyperSonicDrivers/audio/Renderer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::audio
{
    Renderer::Renderer(const size_t buffer_size) : m_buf_size(buffer_size)
    {
    }

    void Renderer::openOutputFile(const std::filesystem::path& path)
    {
        m_out = std::make_unique<files::WAVFile>(path.string(), audio::mixer::eChannelGroup::Unknown, false);
        m_buf.resize(0);
    }

    void Renderer::closeOutputFile() noexcept
    {
        m_out->save_end();
        m_out.reset();
    }

    void Renderer::renderBuffer(IAudioStream* stream)
    {
        if (m_buf.empty())
        {
            m_out->save_prepare(stream->getRate(), stream->isStereo());
            m_buf.resize(m_buf_size);
        }

        const size_t read = stream->readBuffer(m_buf.data(), m_buf.size());
        m_out->save_streaming(m_buf.data(), read);
    }
}
