#include <HyperSonicDrivers/hardware/opl/EmulatedOPL.hpp>
#include <HyperSonicDrivers/audio/streams/OplStream.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <cassert>

namespace HyperSonicDrivers::hardware::opl
{
    EmulatedOPL::EmulatedOPL(const OplType type, const std::shared_ptr<audio::IMixer>& mixer) :
        OPL(mixer, type)
    {
    }

    void EmulatedOPL::startCallbacks(
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan,
        const int timerFrequency
    ) {
        m_stream = std::make_shared<audio::streams::OplStream>(
            this,
            isStereo(),
            m_mixer->getOutputRate(),
            setCallbackFrequency(timerFrequency)
        );

        m_channelId = m_mixer->play(
            group,
            m_stream,
            volume,
            pan
        );

        if (!m_channelId.has_value()) {
            utils::logC("can't start opl playback");
        }
    }

    std::shared_ptr<audio::IAudioStream> EmulatedOPL::getAudioStream() const noexcept
    {
        return m_stream;
    }
}
