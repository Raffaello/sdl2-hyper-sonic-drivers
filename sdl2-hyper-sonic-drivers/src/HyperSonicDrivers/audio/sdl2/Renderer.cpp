#include <HyperSonicDrivers/audio/sdl2/Renderer.hpp>
#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::audio::sdl2
{
    Renderer::Renderer(const uint32_t freq, const uint16_t buffer_size)
    {
        m_mixer = make_mixer<Mixer>(1, freq, buffer_size);
        //if (!m_mixer->init_(Renderer::sdlCallback, this))
        //{
        //    utils::throwLogC<std::runtime_error>("can't init mixer");
        //}
    }

    void Renderer::setOutputFile(const std::filesystem::path& path)
    {
        m_out = std::make_unique<files::WAVFile>(path.string(), audio::mixer::eChannelGroup::Unknown, false);
        /*m_out.open(path, std::ios::binary | std::ios::out);
        if (!m_out.good() || !m_out.is_open())
        {
            utils::throwLogE<>()
            throw std::runtime_error("???");
        }*/
        m_buf.resize(0);
    }

    void Renderer::releaseOutputFile() noexcept
    {
        m_out.reset();
    }

    void Renderer::renderBuffer(IAudioStream* stream)
    {
        if (m_buf.size() == 0) {
            m_out->save_prepare(stream->getRate(), stream->isStereo());
            m_buf.resize(m_mixer->getBufferSize());
        }

        const int read = stream->readBuffer(m_buf.data(), m_buf.size());

        //m_out.write(reinterpret_cast<const char*>(buf.data()), buf.size() * (sizeof(int16_t) / sizeof(char)));
        m_out->save_streaming(m_buf.data(), m_buf.size());
    }

    /*void Renderer::renderBuffer(std::shared_ptr<hardware::opl::EmulatedOPL>& opl)
    {
        renderBuffer(opl->m_stream.get());
    }*/

    /*size_t Renderer::callback(uint8_t* samples, unsigned int len)
    {
        const auto res = m_mixer->callback(samples, len);
        m_out.write(reinterpret_cast<const char*>(samples), len);
        return res;
    }*/

    //void Renderer::sdlCallback(void* userdata, uint8_t* stream, int len)
    //{
    //    memset(stream, 0, len);
    //    //Renderer* r = reinterpret_cast<Renderer*>(userdata);
    //    //assert(r != nullptr);
    //    //r->callback(stream, len);
    //}
}
