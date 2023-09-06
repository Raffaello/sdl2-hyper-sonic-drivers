#pragma once

#include <HyperSonicDrivers/audio/IMixerManager.hpp>

namespace HyperSonicDrivers::audio::sdl2
{
    class MixerManager : public IMixerManager
    {
    public:
        MixerManager(MixerManager&) = delete;
        MixerManager(MixerManager&&) = delete;
        MixerManager& operator=(MixerManager&) = delete;

        MixerManager() = default;
        ~IMixerManager() override = default;

        bool init() override;
        void suspendMixer() override;
        void resumeMixer() override;
    };
}
