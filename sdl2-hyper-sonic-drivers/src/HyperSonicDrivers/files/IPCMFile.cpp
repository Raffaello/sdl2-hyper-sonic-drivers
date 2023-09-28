#include <HyperSonicDrivers/audio/converters/bitsConverter.hpp>
#include <HyperSonicDrivers/files/IPCMFile.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <format>

namespace HyperSonicDrivers::files
{
    void IPCMFile::make_pcm_sound_(const audio::mixer::eChannelGroup group)
    {
        std::shared_ptr<int16_t[]> data;
        uint32_t size = getDataSize();

        switch (getBitsDepth())
        {
        case 8:
            data.reset(audio::converters::convert8to16(getData().get(), size));
            break;
        case 16:
            data = std::reinterpret_pointer_cast<int16_t[]>(getData());
            size >>= 1;
            break;
        default:
            utils::throwLogC<std::invalid_argument>(std::format("bitsDepth = {}, not supported/implemented", getBitsDepth()));
            break;
        }

        if (getChannels() > 2)
            utils::throwLogC<std::runtime_error>(std::format("only mono or stereo PCM files are supported (num_channels = {})", getChannels()));

        m_sound = std::make_shared<audio::PCMSound>(
            group,
            getChannels() == 2,
            getSampleRate(),
            size,
            data
        );
    }
}
