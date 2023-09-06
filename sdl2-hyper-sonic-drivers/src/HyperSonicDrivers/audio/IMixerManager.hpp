#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>

namespace HyperSonicDrivers::audio
{
    class IMixerManager
    {
    public:
        IMixerManager(IMixerManager&) = delete;
        IMixerManager(IMixerManager&&) = delete;
        IMixerManager& operator=(IMixerManager&) = delete;

        IMixerManager() = default;
        virtual ~IMixerManager() = default;

        virtual bool init() = 0;
        virtual void suspendMixer() = 0;
        virtual void resumeMixer() = 0;

        virtual int getNumAudioDevices() = 0;
        virtual void getAudioDevice(const int id) = 0;

        std::shared_ptr<Mixer> getMixer() { return m_mixer; }
    protected:
        std::shared_ptr<IMixer> m_mixer;
        bool m_isMixerSuspended = false;
    };
}
