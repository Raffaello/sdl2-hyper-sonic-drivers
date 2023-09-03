#include <HyperSonicDrivers/utils/endianness.hpp>
#include <algorithm>

// TODO: using c++20 could use std::endianness
namespace HyperSonicDrivers::utils
{
    // could use std::variant and then std::get to access the value.
    // at the moment looks more straightforward in this way.
    // it is an internal union just for helping doing the swap.
    typedef union bytes2
    {
        int16_t i;
        char c[sizeof(int16_t)];
    } bytes2;

    typedef union bytes4
    {
        int32_t i;
        char c[sizeof(int32_t)];
    } bytes4;
     
    constexpr bool is_big_endian()
    {
#ifdef IS_BIG_ENDIAN
        return true;
#else
        return false;
#endif
    }

     inline void swap4(bytes4& n)
     {
        //0,1,2,3 => 3,2,1,0
        std::swap(n.c[0], n.c[3]);
        std::swap(n.c[1], n.c[2]);
    }

    inline void swap2(bytes2& n)
    {
        std::swap(n.c[0], n.c[1]);
    }

    int32_t swapLE32(const int32_t num)
    {
        if constexpr (is_big_endian())
        {
            bytes4 n = { num };
            swap4(n);
            return n.i;
        }
        else
            return num;
    }

    int32_t swapBE32(const int32_t num)
    {
        if constexpr (!is_big_endian())
        {
            bytes4 n = { num };
            swap4(n);
            return n.i;
        }
        else
            return num;
    }

    int16_t swapLE16(const int16_t num)
    {
        if constexpr (is_big_endian())
        {
            bytes2 n = { num };
            swap2(n);
            return n.i;
        }
        else
            return num;
    }

    int16_t swapBE16(const int16_t num)
    {
        if constexpr (!is_big_endian())
        {
            bytes2 n = { num };
            swap2(n);
            return n.i;
        }
        else
            return num;
    }

    uint16_t READ_LE_UINT16(const void* ptr)
    {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);

        return (b[1] << 8) + b[0];
    }

    uint16_t READ_BE_UINT16(const void* ptr)
    {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);

        return (b[0] << 8) + b[1];
    }
}
