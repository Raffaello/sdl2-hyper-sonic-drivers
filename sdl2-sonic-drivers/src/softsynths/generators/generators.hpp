#include <cmath>
#include <limits>
#include <functional>

#ifndef  M_PI
constexpr double  M_PI = 3.14159265358979323846;
#endif // ! M_PI


namespace softsynths {
    namespace generators {
        constexpr double pi2 = 2.0 * M_PI;

        template<typename T> constexpr int32_t unsigned_max = (std::numeric_limits<T>::max() - std::numeric_limits<T>::min());
        static_assert(unsigned_max<int8_t> == std::numeric_limits<uint8_t>::max());
        static_assert(unsigned_max<int16_t> == std::numeric_limits<uint16_t>::max());

        typedef std::function<int8_t(uint32_t, uint32_t)> generatorFunction;

        template<typename T> inline T CLIP(T v)
        {
            if (v < std::numeric_limits<T>::min()) {
                return std::numeric_limits<T>::min();
            }

            if (v > std::numeric_limits<T>::max()) {
                return std::numeric_limits<T>::max();
            }

            return v;
        }

        template<typename T> T generateSquare(const uint32_t x, const uint32_t oscLength)
        {
            return CLIP<T>(
                (x < (oscLength / 2)) ?
                std::numeric_limits<T>::max() :
                std::numeric_limits<T>::min()
                );
        }

        // TODO: this is like a Square review it.
        template<typename T> T generatePulse(const uint32_t x, const uint32_t oscLength)
        {
            return (x < (oscLength / 2)) ?
                std::numeric_limits<T>::max() :
                0;
        }

        template<typename T> T generateSine(const uint32_t x, const uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }

            
            constexpr double pi_coeff = static_cast<double>(std::numeric_limits<T>::max());

            return static_cast<T>(round(pi_coeff * sin(pi2 * x / oscLength)));
        }
        //TODO: start from 0, half max, half+1 min, full 0
        template<typename T> T generateSaw(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);

            if (oscLength == 0) {
                return 0;
            }

            return ((x * unsigned_max<T>) / oscLength) + std::numeric_limits<T>::min();
        }
        //TODO: start from 0, quarter max, half 0, 3quarter min, full 0
        template<typename T> T generateTriangle(const uint32_t x, const uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }

            uint32_t f2 = oscLength / 2;
            return (x < f2) ?
                (x * unsigned_max<T>) / f2 + std::numeric_limits<T>::min() :
                std::numeric_limits<T>::max() - ((x - f2) * unsigned_max<T> / f2);
        }

        const generatorFunction generateWave[] = {
            &generateSquare<int8_t>,
            &generateSine<int8_t>,
            &generateSaw<int8_t>,
            &generateTriangle<int8_t>,
            &generatePulse<int8_t>
        };
    }
}