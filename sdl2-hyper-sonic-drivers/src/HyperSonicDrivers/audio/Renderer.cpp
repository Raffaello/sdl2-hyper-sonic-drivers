#include <HyperSonicDrivers/audio/Renderer.hpp>

namespace HyperSonicDrivers::audio
{
    void Renderer::setOutputFile(const std::filesystem::path& path)
    {
        m_out = std::make_unique<files::WAVFile>(path.string(), audio::mixer::eChannelGroup::Unknown, false);
        m_buf.resize(0);
    }

    void Renderer::releaseOutputFile() noexcept
    {
        m_out.reset();
    }

    void Renderer::renderBuffer(IAudioStream* stream)
    {
        if (m_buf.empty())
        {
            m_out->save_prepare(stream->getRate(), stream->isStereo());
            m_buf.resize(1024);
        }

        const size_t read = stream->readBuffer(m_buf.data(), m_buf.size());
        m_out->save_streaming(m_buf.data(), read);
    }
}
