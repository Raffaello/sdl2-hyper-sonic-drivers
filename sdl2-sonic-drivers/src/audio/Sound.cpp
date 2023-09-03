#include <audio/Sound.hpp>

namespace HyperSonicDrivers::audio
{
    Sound::Sound(const scummvm::Mixer::SoundType soundType, const bool isStereo, const int rate, const uint8_t bitsDepth, const uint32_t dataSize, const std::shared_ptr<uint8_t[]>& data)
        : soundType(soundType), stereo(isStereo), rate(rate), bitsDepth(bitsDepth), dataSize(dataSize), data(data)
    {
    }
}
