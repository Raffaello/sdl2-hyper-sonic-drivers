#include "endianness.hpp"

namespace utils
{
    typedef union bytes2 {
        int16_t i;
        char c[sizeof(int16_t)];
    };

    typedef union bytes4 {
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

    
    inline void swap4(bytes4& n) {
        /*for (int j = 4 - 1, i = 0; i < j; i++, j--) {
            char c = n.c[i];
            n.c[i] = n.c[j];
            n.c[j] = c;
        }*/
        char c = n.c[0];
        n.c[0] = n.c[3]; n.c[3] = c;
        c = n.c[1];
        n.c[1] = n.c[2]; n.c[2] = c;
    }
    inline void swap2(bytes2& n)
    {
        char c = n.c[0];
        n.c[0] = n.c[1]; n.c[1] = c;
    }

    inline int32_t swapLE32(const int32_t num)
    {
        bytes4 n = { num };
        if constexpr (is_big_endian()) {
            swap4(n);
        }

        return n.i;
    }

    inline int32_t swapBE32(const int32_t num)
    {
        bytes4 n = { num };
        if constexpr (!is_big_endian()) {
            swap4(n);
        }

        return n.i;
    }

    inline int16_t swapLE16(const int16_t num)
    {
        bytes2 n = { num };
        if constexpr (is_big_endian()) {
            swap2(n);
        }

        return n.i;
    }

    inline int16_t swapBE16(const int16_t num)
    {
        bytes2 n = { num };
        if constexpr (!is_big_endian()) {
            swap2(n);
        }

        return n.i;
    }

    inline uint16_t READ_LE_UINT16(const void* ptr)
    {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);

        return (b[1] << 8) + b[0];
    }

    inline uint16_t READ_BE_UINT16(const void* ptr)
    {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);

        return (b[0] << 8) + b[1];
    }
}
