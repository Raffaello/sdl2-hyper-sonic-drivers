#pragma once

#include <cstdint>

namespace utils
{
    int32_t swap32LE(const int32_t num);
    int32_t swap32BE(const int32_t num);

    int16_t swap16BE(const int16_t num);
    int16_t swap16LE(const int16_t num);
    
}
