#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/audio/converters/SimpleRateConverter.hpp>
#include <HyperSonicDrivers/audio/converters/LinearRateConverter.hpp>
#include <HyperSonicDrivers/audio/converters/CopyRateConverter.hpp>
#include <cstdint>

namespace HyperSonicDrivers::audio::converters
{
    template<bool stereo, bool reverseStereo>
    IRateConverter* makeIRateConverter(const uint32_t inrate, const uint32_t outrate)
    {
        if (inrate != outrate) {
            if ((inrate % outrate) == 0 && (inrate < 65536))
            {
                return new SimpleRateConverter<stereo, reverseStereo>(inrate, outrate);
            }
            else
            {
                return new LinearRateConverter<stereo, reverseStereo>(inrate, outrate);
            }
        }
        else
        {
            return new CopyRateConverter<stereo, reverseStereo>();
        }
    }

    /**
     * Create and return a RateConverter object for the specified input and output rates.
     */
    IRateConverter* makeIRateConverter(const uint32_t inrate, const uint32_t outrate, const bool stereo, const bool reverseStereo)
    {
        if (stereo)
        {
            if (reverseStereo)
                return makeIRateConverter<true, true>(inrate, outrate);
            else
                return makeIRateConverter<true, false>(inrate, outrate);
        }
        else
            return makeIRateConverter<false, false>(inrate, outrate);
    }
}
