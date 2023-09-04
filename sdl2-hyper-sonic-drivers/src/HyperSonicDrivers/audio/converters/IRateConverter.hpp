#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/scummvm/AudioStream.hpp> // TODO redo it, avoid to use scummvm namespace

namespace HyperSonicDrivers::audio::converters
{
    class IRateConverter
    {
    public:
        IRateConverter() = default;
        virtual ~IRateConverter() = default;

        /**
         * @return Number of sample pairs written into the buffer.
         */
        virtual int flow(HyperSonicDrivers::audio::scummvm::AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) = 0;

        virtual int drain(int16_t* obuf, uint32_t osamp, uint16_t vol) = 0;
    };

    IRateConverter* makeRateConverter(uint32_t inrate, uint32_t outrate, bool stereo, bool reverseStereo = false);
}