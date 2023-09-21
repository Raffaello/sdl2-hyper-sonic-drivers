#include <format>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/audio/streams/OplStream.hpp>


namespace HyperSonicDrivers::hardware::opl
{
    using utils::logE;

    OPL::OPL(const std::shared_ptr<audio::IMixer>& mixer, const OplType type) :
        IHardware(mixer),
        type(type)
    {
    }

    void OPL::start(
        const std::shared_ptr<TimerCallBack>& callback,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan,
        const int timerFrequency)
    {
        IHardware::start(callback, group, volume, pan, timerFrequency);
    }

    void OPL::startCallbacks(
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan,
        const int timerFrequency
    ) {
        setAudioStream(std::make_shared<audio::streams::OplStream>(
            this,
            isStereo(),
            m_mixer->getOutputRate(),
            setCallbackFrequency(timerFrequency)
        ));

        m_channelId = m_mixer->play(
            group,
            getAudioStream(),
            volume,
            pan
        );

        if (!m_channelId.has_value()) {
            utils::logC("can't start opl playback");
        }
    }
}
