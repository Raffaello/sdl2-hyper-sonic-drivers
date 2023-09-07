#pragma once

#include <limits>

namespace HyperSonicDrivers::audio::mixer
{
    constexpr int Channel_max_volume = std::numeric_limits<uint8_t>::max();
    constexpr int Mixer_max_volume = std::numeric_limits<uint8_t>::max();
}
