#include <HyperSonicDrivers/audio/Sound.hpp>
#include <HyperSonicDrivers/audio/converters/SimpleBitsConverter.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::audio
{
    Sound::Sound(
        const mixer::eChannelGroup group,
        const bool isStereo,
        const uint32_t freq,
        const uint32_t dataSize,
        const std::shared_ptr<int16_t[]> &data) :
        group(group),
        stereo(isStereo),
        freq(freq),
        m_dataSize(dataSize),
        m_data(data)
    {
        if (data == nullptr)
            utils::throwLogC<std::runtime_error>("passed nullptr data");
        
    }
}
