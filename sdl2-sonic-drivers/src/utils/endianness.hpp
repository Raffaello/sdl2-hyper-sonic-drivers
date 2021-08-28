#pragma once

#include <cstdint>

namespace utils
{
    extern inline int32_t swapLE32(const int32_t num);
    extern inline int32_t swapBE32(const int32_t num);
    extern inline int16_t swapLE16(const int16_t num);
    extern inline int16_t swapBE16(const int16_t num);

    extern inline uint16_t READ_LE_UINT16(const void* ptr);
    extern inline uint16_t READ_BE_UINT16(const void* ptr);
}
