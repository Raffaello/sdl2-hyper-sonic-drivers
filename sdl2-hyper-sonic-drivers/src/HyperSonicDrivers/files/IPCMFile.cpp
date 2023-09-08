#include <HyperSonicDrivers/files/IPCMFile.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <format>
#include <HyperSonicDrivers/audio/converters/SimpleBitsConverter.hpp>

namespace HyperSonicDrivers::files
{
    void IPCMFile::make_sound_(const audio::mixer::eChannelGroup group)
    {
        std::shared_ptr<int16_t[]> data;
        uint32_t size = getDataSize();
        switch (getBitsDepth())
        {
        case 8:
            data.reset(audio::converters::SimpleBitsConverter::convert8to16(getData().get(), size));
            break;
        case 16:
            data = std::reinterpret_pointer_cast<int16_t[]>(getData());
            size >>= 1;
            break;
        default:
            utils::throwLogC<std::invalid_argument>(std::format("bitsDepth = {}, not supported/implemented", getBitsDepth()));
            break;
        }

        m_sound = std::make_shared<audio::Sound>(
            group,
            getChannels() == 2,
            getSampleRate(),
            size,
            data
        );
    }
}
