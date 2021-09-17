#pragma once

#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>


namespace utils
{
    template<typename T1, typename T2, typename T3, typename T4>
    constexpr uint32_t MKID_BE(T1 a, T2  b, T3  c, T4  d) { return a | b << 8 | c << 16 | d << 24; }

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
    template<typename T, size_t N>
    inline void ARRAYCLEAR(T(&array)[N], const T& value = T()) {
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

    template<typename T>
    inline T getMicro()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    inline void delayMillis(const unsigned int delaytime)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delaytime));
    }

    inline void delayMicro(const unsigned int delaytime)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(delaytime));
    }

    /// <summary>
    /// Variable length quantity decoding algorithm
    /// </summary>
    /// <param name="buf">the max 4 bytes array to decode</param>
    /// <param name="out_value">the resulting decoded value</param>
    /// <returns>byte reads</returns>
    int decode_VLQ(const uint8_t buf[], uint32_t& out_value);

    /// <summary>
    /// return the value of 2^coeff
    /// </summary>
    /// <param name="coeff">coefficient to elevate 2</param>
    /// <returns>2^coeff</returns>
    int powerOf2(const int coeff);

    /// <summary>
    /// XMI Variable length quantuty decoding algorithm
    /// </summary>
    /// <param name="buf">the max 4 bytes array to decode</param>
    /// <param name="out_value">the resulting decoded value</param>
    /// <returns>byte reads</returns>
    int decode_xmi_VLQ(const uint8_t buf[], uint32_t& out_value);
    
    int decode_xmi_VLQ(std::vector<uint8_t>& v, int index, uint32_t& out_value);
}
