#include <HyperSonicDrivers/audio/PCMSound.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::audio
{
    PCMSound::PCMSound(
        const mixer::eChannelGroup group,
        const bool isStereo,
        const uint32_t freq,
        const uint32_t dataSize,
        const std::shared_ptr<int16_t[]> &data) :
        group(group),
        stereo(isStereo),
        freq(freq),
        dataSize(dataSize),
        data(data)
    {
        if (data == nullptr)
            utils::throwLogC<std::runtime_error>("passed nullptr data");
    }
}
