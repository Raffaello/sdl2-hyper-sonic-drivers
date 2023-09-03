#pragma once

#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>

namespace utils
{
    template<typename T1, typename T2, typename T3, typename T4>
    constexpr uint32_t MKID_BE(T1 a, T2  b, T3  c, T4  d) { return a | b << 8 | c << 16 | d << 24; }

    template <class T>
    static inline T CLIP(const T& value, const T& min, const T& max)
    {
        //return value < min ? min : value > max ? max : value;
        return std::clamp<T>(value, min, max);
    }

    /**
     * Clear an array using the default or provided value.
     * TODO replace with std::fill and std::array
     * @deprecated
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
    /// return the value of 2^coeff
    /// </summary>
    /// <param name="coeff">coefficient to elevate 2</param>
    /// <returns>2^coeff</returns>
    inline int powerOf2(const int coeff)
    {
        return 1 << coeff;
    }

    /// <summary>
    /// Variable length quantity decoding algorithm
    /// </summary>
    /// <param name="buf">the max 4 bytes array to decode</param>
    /// <param name="out_value">the resulting decoded value</param>
    /// <returns>byte reads</returns>
    int decode_VLQ(const uint8_t buf[], uint32_t& out_value);

    /// <summary>
    /// XMI Variable length quantity decoding algorithm
    /// </summary>
    /// <param name="buf">the max 4 bytes array to decode</param>
    /// <param name="out_value">the resulting decoded value</param>
    /// <returns>byte reads</returns>
    int decode_xmi_VLQ(const uint8_t buf[], uint32_t& out_value);

    /// <summary>
    /// Converts a uint8_t vector to a string.
    /// </summary>
    std::string chars_vector_to_string(const std::vector<uint8_t>::const_iterator& begin, const std::vector<uint8_t>::const_iterator& end);

    /// <summary>
    /// wrapper, helper function
    /// </summary>
    std::string chars_vector_to_string(const std::vector<uint8_t>& e);
    /// <summary>
    /// Skip first char
    /// </summary>
    /// <param name="e"></param>
    /// <returns></returns>
    std::string chars_vector_to_string_skip_first(const std::vector<uint8_t>& e);
}
