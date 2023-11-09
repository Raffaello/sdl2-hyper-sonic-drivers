#include <cstdint>
#include <HyperSonicDrivers/utils/sound.hpp>
#include <HyperSonicDrivers/audio/converters/LinearRateConverter.hpp>

namespace HyperSonicDrivers::utils
{
    std::shared_ptr<audio::PCMSound> makeMono(const std::shared_ptr<audio::PCMSound>& sound)
    {
        if (!sound->stereo)
            return sound;

        const uint32_t monoDataSize = sound->dataSize / 2;

        auto monoData = std::make_shared<int16_t[]>(monoDataSize);
        for (uint32_t i = 0, j = 0; i < sound->dataSize; i += 2, j++)
        {
            monoData[j] = (sound->data[i] + sound->data[i + 1]) / 2;
        }

        return std::make_shared<audio::PCMSound>(sound->group, false, sound->freq, monoDataSize, monoData);
    }

    std::shared_ptr<audio::PCMSound> makeStereo(const std::shared_ptr<audio::PCMSound>& sound)
    {
        if (sound->stereo)
            return sound;

        const uint32_t stereoDataSize = sound->dataSize * 2;
        auto stereoData = std::make_shared<int16_t[]>(stereoDataSize);
        for (uint32_t i = 0, j = 0; i < sound->dataSize; i++, j += 2)
        {
            stereoData[j] = stereoData[j + 1] = sound->data[i];
        }

        return std::make_shared<audio::PCMSound>(sound->group, true, sound->freq, stereoDataSize, stereoData);
    }

    std::shared_ptr<audio::PCMSound> append(
        const std::shared_ptr<audio::PCMSound>& sound1,
        const std::shared_ptr<audio::PCMSound>& sound2)
    {
        std::shared_ptr<audio::PCMSound> s2;

        if (sound1->stereo)
            s2 = makeStereo(sound2);
        else
            s2 = makeMono(sound2);

        if (sound1->freq != sound2->freq)
        {
            // TODO:
            // use covert frequency/ sample rate libraries for this ?
            utils::throwLogC<std::runtime_error>("different frequency, not implemented yet");
        }

        const uint32_t dataSize = sound1->dataSize + s2->dataSize;
        auto data = std::make_shared<int16_t[]>(dataSize);

        memcpy(data.get(), sound1->data.get(), sound1->dataSize * sizeof(int16_t));
        memcpy(&data[sound1->dataSize], s2->data.get(), s2->dataSize * sizeof(int16_t));

        return std::make_shared<audio::PCMSound>(sound1->group, sound1->stereo, sound1->freq, dataSize, data);
    }
}
