#pragma once

#include <cstdint>

namespace utils
{
    extern inline int32_t readLE32(const int32_t num);
    extern inline int32_t read32BE(const int32_t num);

    extern inline int16_t read16BE(const int16_t num);
    extern inline int16_t read16LE(const int16_t num);
}
