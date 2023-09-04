#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/audio/converters/SimpleRateConverter.hpp>
#include <cstdint>

namespace HyperSonicDrivers::audio::converters
{
    template<bool stereo, bool reverseStereo>
    IRateConverter* makeRateConverter(uint32_t inrate, uint32_t outrate) {
        if (inrate != outrate) {
            if ((inrate % outrate) == 0 && (inrate < 65536)) {
                return new SimpleRateConverter<stereo, reverseStereo>(inrate, outrate);
            }
            else {
                return new LinearRateConverter<stereo, reverseStereo>(inrate, outrate);
            }
        }
        else {
            return new CopyRateConverter<stereo, reverseStereo>();
        }
    }

    /**
     * Create and return a RateConverter object for the specified input and output rates.
     */
    IRateConverter* makeRateConverter(uint32_t inrate, uint32_t outrate, bool stereo, bool reverseStereo) {
        if (stereo) {
            if (reverseStereo)
                return makeRateConverter<true, true>(inrate, outrate);
            else
                return makeRateConverter<true, false>(inrate, outrate);
        }
        else
            return makeRateConverter<false, false>(inrate, outrate);
    }
}
