#include <cmath>
#include <limits>
#include <functional>

#ifndef M_PI
constexpr double M_PI = 3.14159265358979323846;
#endif // ! M_PI

#ifndef M_2PI
constexpr double M_2PI = 2.0 * M_PI;
#endif // ! M_2PI

namespace softsynths {
    namespace generators {

        template<typename T> constexpr int32_t unsigned_max = (std::numeric_limits<T>::max() - std::numeric_limits<T>::min());
        static_assert(unsigned_max<int8_t> == std::numeric_limits<uint8_t>::max());
        static_assert(unsigned_max<int16_t> == std::numeric_limits<uint16_t>::max());

        
        //typedef std::function<int8_t(uint32_t, uint32_t)> generatorFunction;

        /*template<typename T> inline T CLIP(T v)
        {
            if (v < std::numeric_limits<T>::min()) {
                return std::numeric_limits<T>::min();
            }

            if (v > std::numeric_limits<T>::max()) {
                return std::numeric_limits<T>::max();
            }

            return v;
        }*/

        template<typename T> T generateSquare(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);
            return (x < (oscLength / 2)) ?
                std::numeric_limits<T>::max():
                std::numeric_limits<T>::min();
        }

        template<typename T> T generatePulse(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);
            return (x < (oscLength / 2)) ?
                std::numeric_limits<T>::max() :
                0;
        }

        template<typename T> T generateSine(const uint32_t x, const uint32_t oscLength)
        {
            static_assert(std::numeric_limits<T>::is_integer);
            if (oscLength == 0) {
                return 0;
            }

            constexpr double pi_coeff = static_cast<double>(std::numeric_limits<T>::max());

            return static_cast<T>(round(pi_coeff * sin(M_2PI * x / oscLength)));
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

        // TODO: use uint32_t for wave generators? and just rescale to the typename T
        template<typename T>
        const std::function<T(uint32_t, uint32_t)> generateWave[] = {
            &generateSquare<T>,
            &generateSine<T>,
            &generateSaw<T>,
            &generateTriangle<T>,
            &generatePulse<T>
        };
    }
}