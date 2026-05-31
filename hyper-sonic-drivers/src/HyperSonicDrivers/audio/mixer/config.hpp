#pragma once

#include <limits>

namespace HyperSonicDrivers::audio::mixer
{
constexpr int CHANNEL_MAX_VOLUME = std::numeric_limits<uint8_t>::max();
constexpr int MIXER_MAX_VOLUME   = std::numeric_limits<uint8_t>::max();
}    // namespace HyperSonicDrivers::audio::mixer
