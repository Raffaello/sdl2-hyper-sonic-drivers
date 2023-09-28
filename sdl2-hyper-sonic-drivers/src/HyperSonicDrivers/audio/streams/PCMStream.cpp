#include <HyperSonicDrivers/audio/streams/PCMStream.hpp>
#include <HyperSonicDrivers/utils/endianness.hpp>
#include <cassert>

namespace HyperSonicDrivers::audio::streams
{
    using utils::readLE_uint16;

    PCMStream::PCMStream(const std::shared_ptr<PCMSound>& sound)
        : m_sound(sound)
    {
    }

    size_t PCMStream::readBuffer(int16_t* buffer, const size_t numSamples)
    {
        const size_t rest = (m_sound->dataSize - m_curPos);
        const size_t remaining = std::min<>(numSamples, rest);
 
        for (size_t i = 0; i < remaining; i++)
            buffer[i] = m_sound->data[m_curPos++];

        assert(m_curPos <= m_sound->dataSize);
        return remaining;
    }

    bool PCMStream::isStereo() const
    {
        return m_sound->stereo;
    }

    uint32_t PCMStream::getRate() const
    {
        return m_sound->freq;
    }

    bool PCMStream::endOfData() const
    {
        return m_curPos == m_sound->dataSize;
    }

    std::weak_ptr<PCMSound> PCMStream::getSound() const noexcept
    {
        return m_sound;
    }
}
