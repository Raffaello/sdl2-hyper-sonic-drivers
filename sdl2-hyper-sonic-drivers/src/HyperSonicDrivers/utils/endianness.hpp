#pragma once

#include <cstdint>

namespace HyperSonicDrivers::utils
{
    // MOVBE instruction could do the swap automatically.

    int32_t swapLE32(const int32_t num) noexcept;
    int32_t swapBE32(const int32_t num) noexcept;
    int16_t swapLE16(const int16_t num) noexcept;
    int16_t swapBE16(const int16_t num) noexcept;

    uint16_t READ_LE_UINT16(const void* ptr) noexcept;
    uint16_t READ_BE_UINT16(const void* ptr) noexcept;
}
