#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/audio/converters/SimpleRateConverter.hpp>
#include <HyperSonicDrivers/audio/converters/LinearRateConverter.hpp>
#include <HyperSonicDrivers/audio/converters/CopyRateConverter.hpp>
#include <cstdint>
#include <memory>

namespace HyperSonicDrivers::audio::converters
{
template <bool stereo, bool reverseStereo>
std::unique_ptr<IRateConverter> makeIRateConverter(const uint32_t in_rate, const uint32_t outrate)
{
    if (in_rate != outrate)
    {
        if ((in_rate % outrate) == 0 && (in_rate < 65536))
            return std::make_unique<SimpleRateConverter<stereo, reverseStereo>>(in_rate, outrate);
        else
            return std::make_unique<LinearRateConverter<stereo, reverseStereo>>(in_rate, outrate);
    }
    else
        return std::make_unique<CopyRateConverter<stereo, reverseStereo>>();
}

/**
 * Create and return a RateConverter object for the specified input and output rates.
 */
std::unique_ptr<IRateConverter> makeIRateConverter(const uint32_t in_rate, const uint32_t outrate, const bool stereo, const bool reverseStereo)
{
    if (stereo)
    {
        if (reverseStereo)
            return makeIRateConverter<true, true>(in_rate, outrate);
        else
            return makeIRateConverter<true, false>(in_rate, outrate);
    }
    else
        return makeIRateConverter<false, false>(in_rate, outrate);
}
}    // namespace HyperSonicDrivers::audio::converters
