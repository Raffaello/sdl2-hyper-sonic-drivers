#pragma once

#include <cstdint>
#include <chrono>
#include <thread>

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

    template<typename T>
    inline T getMillis()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    inline void delayMillis(const unsigned int delaytime)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delaytime));
    }

    inline void delayMicro(const unsigned int delaytime)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(delaytime));
    }
}
