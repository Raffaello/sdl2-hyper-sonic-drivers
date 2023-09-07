#include <HyperSonicDrivers/audio/streams/SoundStream.hpp>
#include <HyperSonicDrivers/utils/endianness.hpp>
#include <cassert>

namespace HyperSonicDrivers::audio::streams
{
    using utils::READ_LE_UINT16;

    SoundStream::SoundStream(const std::shared_ptr<Sound>& sound)
        : m_sound(sound), m_bitsFactor(m_sound->bitsDepth == 16 ? 2 : 1)
    {
    }

    SoundStream::~SoundStream()
    {
    }

    size_t SoundStream::readBuffer(int16_t* buffer, const size_t numSamples)
    {
        assert(m_sound->dataSize % m_bitsFactor == 0);

        const size_t rest = (m_sound->dataSize - m_curPos) / m_bitsFactor;
        const size_t remaining = std::min<uint32_t>(numSamples, rest);

        for (size_t i = 0; i < remaining; i++)
        {
            // TODO convert Audio stream before playback? (yes when loading the file)
            if (m_sound->bitsDepth == 8)
            {
                buffer[i] = static_cast<int16_t>((m_sound->data[m_curPos++] - 128) * 128);
            }
            else {
                buffer[i] = READ_LE_UINT16(&m_sound->data[m_curPos]);
                m_curPos += 2;
            }
        }

        assert(m_curPos <= m_sound->dataSize);
        return remaining;
    }

    bool SoundStream::isStereo() const
    {
        return m_sound->stereo;
    }

    uint32_t SoundStream::getRate() const
    {
        return m_sound->freq;
    }

    bool SoundStream::endOfData() const
    {
        return m_curPos == m_sound->dataSize;
    }

    std::weak_ptr<Sound> SoundStream::getSound() const noexcept
    {
        return m_sound;
    }
}
