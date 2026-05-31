#include <HyperSonicDrivers/audio/converters/bitsConverter.hpp>
#include <HyperSonicDrivers/files/IPCMFile.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <format>

namespace HyperSonicDrivers::files
{
    void IPCMFile::make_pcm_sound_(const audio::mixer::eChannelGroup group)
    {
        if (m_channels > 2)
            utils::throwLogC<std::runtime_error>(std::format("only mono or stereo PCM files are supported (num_channels = {})", m_channels));

        std::shared_ptr<int16_t[]> data;
        uint32_t size = m_dataSize;

        switch (m_bitsDepth)
        {
        case 8:
            data.reset(audio::converters::convert8to16(m_data.get(), size));
            break;
        case 16:
            data = std::reinterpret_pointer_cast<int16_t[]>(m_data);
            size >>= 1;
            break;
        default:
            utils::throwLogC<std::invalid_argument>(std::format("bitsDepth = {}, not supported/implemented", m_bitsDepth));
            break;
        }

        m_sound = std::make_shared<audio::PCMSound>(
            group,
            m_channels == 2,
            m_sampleRate,
            size,
            data
        );
    }
}
