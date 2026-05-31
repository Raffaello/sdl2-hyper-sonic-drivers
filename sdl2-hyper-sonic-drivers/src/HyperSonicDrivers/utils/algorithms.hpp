#pragma once

#include <cstdint>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <limits>
#include <cassert>

#if HAS_SDL3
#include <SDL3/SDL_timer.h>
#else
#include <SDL2/SDL_timer.h>
#endif

namespace HyperSonicDrivers::utils
{
template <typename T>
constexpr double RMS(const std::vector<T>& samples)
{
    double           sum    = 0.0;
    constexpr auto   range  = std::numeric_limits<T>::max() /* - std::numeric_limits<T>::min()*/;
    constexpr double range2 = range * range;
    for (const auto& s : samples)
    {
        sum += (s * s) / range2;
    }
    return sqrt(sum / static_cast<double>(samples.size()));
}

double dBFS(const double rms);

template <typename T>
double dBFS(const std::vector<T>& samples)
{
    return dBFS(RMS(samples));
}

template <typename T1, typename T2, typename T3, typename T4>
constexpr uint32_t MKID_BE(T1 a, T2 b, T3 c, T4 d)
{
    return a | b << 8 | c << 16 | d << 24;
}

template <typename T>
inline T getMillis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

template <typename T>
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
    // TODO: create a Timer cross-os class/method instead

    // std::this_thread::sleep_for(std::chrono::microseconds(delaytime));
#if HAS_SDL3
    {
        SDL_DelayNS(delaytime * 1'000);
    }
#else
    {
        constexpr uint64_t MICROSECONDS = 1'000'000;

        assert(SDL_GetPerformanceCounter() >= MICROSECONDS);

        const static uint64_t perf_freq     = SDL_GetPerformanceFrequency() / MICROSECONDS;    // microseconds
        const uint64_t        current_ticks = SDL_GetPerformanceCounter();
        const uint64_t        wait_until    = current_ticks + static_cast<uint64_t>(delaytime) * perf_freq;

        // Yield the CPU for the bulk of the wait (e.g. 250 ms MIDI chunks)
        // then busy-wait only the last 2 ms for precision.
        constexpr unsigned int BUSYWAIT_MARGIN_US = 2'000u;
        if (delaytime > BUSYWAIT_MARGIN_US)
            delayMillis((delaytime - BUSYWAIT_MARGIN_US) / MICROSECONDS);

        while (SDL_GetPerformanceCounter() < wait_until)
        {
        }
    }
#endif
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
}    // namespace HyperSonicDrivers::utils
