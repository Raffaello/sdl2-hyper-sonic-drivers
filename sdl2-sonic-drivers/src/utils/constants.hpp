#pragma once

#include <cmath>
#include <numbers>

namespace utils
{
#ifdef M_PI
    #undef M_PI
#endif // M_PI
    //constexpr double M_PI = 3.14159265358979323846;
    constexpr double M_PI = std::numbers::pi;

#ifdef M_2PI
    #undef M_2PI
#endif // M_2PI
    constexpr double M_2PI = 2.0 * M_PI;

    //constexpr double PI = 3.141592653589793238462643;
    //constexpr double PI2 = PI * 2.0;
    constexpr double PI = M_PI;
    constexpr double PI2 = M_2PI;

    const char* getVersion();
    int getVersionMajor();
    int getVersionMinor();
    int getVersionPatch();
}
