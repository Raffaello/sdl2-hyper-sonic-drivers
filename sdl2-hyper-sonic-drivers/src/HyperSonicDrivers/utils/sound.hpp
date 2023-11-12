#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/PCMSound.hpp>

namespace HyperSonicDrivers::utils
{
    /**
    * @returns a new mono PCMSound if the parameter is stereo
    *          otherwise returns the parameter itself.
    **/
    std::shared_ptr<audio::PCMSound> makeMono(const std::shared_ptr<audio::PCMSound>& sound);

    /**
    * @returns a new stereo PCMSound if the parameter is mono
    *          otherwise returns the parameter itself.
    **/
    std::shared_ptr<audio::PCMSound> makeStereo(const std::shared_ptr<audio::PCMSound>& sound);
 
    /**
    * @returns sound1 + sound2.
    *          it converts sound2 to the same freq and mono/stereo of sound1
    **/
    std::shared_ptr<audio::PCMSound> append(
        const std::shared_ptr<audio::PCMSound>& sound1,
        const std::shared_ptr<audio::PCMSound>& sound2);

    uint32_t duration_ms(const std::shared_ptr<audio::PCMSound>& sound);
    uint32_t ms_toPos(const uint32_t, const std::shared_ptr<audio::PCMSound>& sound);
}
