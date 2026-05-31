#pragma once

#include <memory>
#include <string>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/files/VOCFile.hpp>
#include <HyperSonicDrivers/files/WAVFile.hpp>

namespace HyperSonicDrivers::files
{
    template<class T, typename... Args>
    std::shared_ptr<audio::PCMSound> loadSoundFromFile(const std::string& filename, Args... args)
    {
        return T(filename, args...).getSound();
    }
}
