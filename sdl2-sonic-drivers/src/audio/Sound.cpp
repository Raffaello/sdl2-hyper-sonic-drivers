#include <audio/Sound.hpp>
#include <utils/endianness.hpp>


namespace audio
{
    using utils::READ_LE_UINT16;
    using audio::scummvm::SoundHandle;

    Sound::Sound(const scummvm::Mixer::SoundType soundType, const bool isStereo, const int rate, const uint8_t bitsDepth, const uint32_t dataSize, const std::shared_ptr<uint8_t[]> data)
        : soundType(soundType), stereo(isStereo), rate(rate), bitsDepth(bitsDepth), dataSize(dataSize), data(data)
    {
        //_bitsFactor = bitsDepth == 16 ? 2 : 1;
    }
}
