#pragma once

#include <cstdint>

namespace utils
{
    extern inline int32_t swap32LE(const int32_t num);
    extern inline int32_t swap32BE(const int32_t num);

    extern inline int16_t swap16BE(const int16_t num);
    extern inline int16_t swap16LE(const int16_t num);
    
}
