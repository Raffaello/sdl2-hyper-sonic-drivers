#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/PCMSound.hpp>

namespace HyperSonicDrivers::utils
{
    std::shared_ptr<audio::PCMSound> makeMono(const std::shared_ptr<audio::PCMSound>& sound);
    std::shared_ptr<audio::PCMSound> makeStereo(const std::shared_ptr<audio::PCMSound>& sound);
    
    std::shared_ptr<audio::PCMSound> append(
        const std::shared_ptr<audio::PCMSound>& sound1,
        const std::shared_ptr<audio::PCMSound>& sound2);
}