#pragma once

#include <cstdint>

namespace utils
{
    /**
     * Euclidean algorithm to compute the greatest common divisor.
     */
    template<class T>
    T gcd(T a, T b)
    {
        // Note: We check for <= instead of < to avoid spurious compiler
        // warnings if T is an unsigned type, i.e. warnings like "comparison
        // of unsigned expression < 0 is always false".
        if (a <= 0) {
            a = -a;
        }

        if (b <= 0) {
            b = -b;
        }

        while (a > 0)
        {
            T tmp = a;
            a = b % a;
            b = tmp;
        }

        return b;
    }

    template <class T>
    static inline T CLIP(const T& value, const T& min, const T& max)
    {
        return value < min ? min : value > max ? max : value;
    }

    /**
     * Clear an array using the default or provided value.
     */
    template<typename T, size_t N> inline void ARRAYCLEAR(T(&array)[N], const T& value = T()) {
        T* ptr = array;
        size_t n = N;
        while (n--) {
            *ptr++ = value;
        }
    }
}
