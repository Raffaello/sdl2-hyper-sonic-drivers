#pragma once
#include <cmath>
#include <limits>
#include <functional>
//#include <unordered_map>

#ifndef M_PI
constexpr double M_PI = 3.14159265358979323846;
#endif // ! M_PI

#ifndef M_2PI
constexpr double M_2PI = 2.0 * M_PI;
#endif // ! M_2PI

namespace softsynths
{
    namespace generators
    {
        template<typename T> constexpr int32_t unsigned_max = (std::numeric_limits<T>::max() - std::numeric_limits<T>::min());
        static_assert(unsigned_max<int8_t> == std::numeric_limits<uint8_t>::max());
        static_assert(unsigned_max<int16_t> == std::numeric_limits<uint16_t>::max());
        static_assert(unsigned_max<uint8_t> == std::numeric_limits<uint8_t>::max());
        static_assert(unsigned_max<uint16_t> == std::numeric_limits<uint16_t>::max());
        
        template<typename T> constexpr int32_t mid = (std::numeric_limits<T>::max() + std::numeric_limits<T>::min()) / 2;
        static_assert(mid<int8_t> == mid<int16_t>);
        static_assert(mid<uint8_t> == 0x7F);
        static_assert(mid<uint16_t> == 0x7FFF);

        template<typename T> T generateSquare(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);
            return (x < (oscLength / 2)) ?
                std::numeric_limits<T>::max():
                std::numeric_limits<T>::min();
        }

        template<typename T> T generateSine(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);
            if (oscLength == 0) {
                return 0;
            }

            constexpr T pi_coeff = unsigned_max<T> / 2;

            return static_cast<T>(round(pi_coeff * (sin(M_2PI * x / oscLength))) + mid<T>);
        }
        
        template<typename T> T generateSaw(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);
            if (oscLength == 0) {
                return 0;
            }

            return ((x * unsigned_max<T>) / oscLength) + std::numeric_limits<T>::min();
        }
    
        template<typename T> T generateTriangle(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);
            if (oscLength == 0) {
                return 0;
            }

            uint32_t f2 = oscLength / 2;
                
            return (x < f2) ?
                (x * unsigned_max<T>) / f2 + std::numeric_limits<T>::min() :
                std::numeric_limits<T>::max() - ((x - f2) * unsigned_max<T> / f2);
        }

        template<typename T> const std::function<T(uint32_t, uint32_t)> generateWave[] = {
            &generateSquare<T>,
            &generateSine<T>,
            &generateSaw<T>,
            &generateTriangle<T>,
        };

        /// <summary>
        /// Wave From Generators
        /// </summary>
        enum class eWaveForm
        {
            SQUARE = 0,
            SINE,
            SAW,
            TRIANGLE,
        };
        

        template<typename T> std::unordered_map<const eWaveForm, const std::function<T(uint32_t, uint32_t)>> generateWave2 {
            {eWaveForm::SQUARE, &generateSquare<T>},
            {eWaveForm::SINE, &generateSine<T>},
            {eWaveForm::SAW, &generateSaw<T>},
            {eWaveForm::TRIANGLE, &generateTriangle<T>},
        };
    }
}
