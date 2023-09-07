#include <HyperSonicDrivers/drivers/PCMDriver.hpp>
#include <HyperSonicDrivers/audio/scummvm/MixerImpl.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>

namespace HyperSonicDrivers::drivers
{
    using audio::scummvm::Mixer;
    using audio::streams::SoundStream;

    PCMDriver::PCMDriver(const std::shared_ptr<audio::IMixer>& mixer, const uint8_t max_channels) :
        m_mixer(mixer)
    {
        m_max_streams = std::min(m_mixer->max_channels, max_channels);
        m_soundStreams.resize(m_max_streams);
    }

    bool PCMDriver::isPlaying() const noexcept
    {
        for (int i = 0; i < m_max_streams; ++i)
        {
            const auto& ss = m_soundStreams[i];
            if(ss != nullptr && !ss->isEnded())
                return true;
        }

        return false;
    }

    bool PCMDriver::isPlaying(const std::shared_ptr<audio::Sound>& sound) const noexcept
    {
        // TODO: should be returned the soundHandle or soundID in play method to be used later on?
        // BODY: so here it can be addressed in constant time instead of searching for sound in the slots?
        for (int i = 0; i < m_max_streams; i++)
        {
            const auto& ss = m_soundStreams[i];
            if (ss != nullptr
                && ss->getSound().lock() == sound
                && !ss->isEnded()
                )
                return true;
        }

        return false;
    }

    void PCMDriver::play(const std::shared_ptr<audio::Sound>& sound, const uint8_t volume, const int8_t pan)
    {
        // TODO: this method is not thread-safe at the moment.
        int cur_stream;
        
        // find first free slot
        for (cur_stream = 0; cur_stream < m_max_streams ; ++cur_stream)
        {
            const auto& ss = m_soundStreams[cur_stream];
            if (ss == nullptr || ss->isEnded())
                break;
        }

        if (cur_stream == m_max_streams)
            return;

        m_soundStreams[cur_stream] = std::make_shared<SoundStream>(SoundStream(sound));

        // TODO: could be autofree stream and create directly on the playStream method simplified all?
        // BODY: Yes, but loosing the handle for checking if is it playing.
        // BODY: alternatively could be stored the ID?
        /*m_mixer->playStream(
            sound->soundType,
            m_soundStreams[cur_stream]->getSoundHandlePtr(),
            m_soundStreams[cur_stream].get(),
            -1,
            volume,
            balance,
            false
        );*/

        auto channelId = m_mixer->play(
            sound->group,
            m_soundStreams[cur_stream],
            volume,
            pan,
            false
        );
    }

    /*inline bool PCMDriver::isSoundHandleActive(const int index) const noexcept
    {
        return nullptr != m_soundStreams[index] &&
            m_mixer->isSoundHandleActive(*m_soundStreams[index]->getSoundHandlePtr());
    }*/
}
