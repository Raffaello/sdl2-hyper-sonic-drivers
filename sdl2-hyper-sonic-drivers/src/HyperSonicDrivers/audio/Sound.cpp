#include <HyperSonicDrivers/audio/Sound.hpp>

namespace HyperSonicDrivers::audio
{
    Sound::Sound(const mixer::eChannelGroup group,
        const bool isStereo, const uint32_t freq,
        const uint8_t bitsDepth,
        const std::shared_ptr<std::vector<uint8_t>>& data) :
        group(group),
        stereo(isStereo),
        bitsDepth(bitsDepth),
        freq(freq),
        data(data)
    {
    }
}
