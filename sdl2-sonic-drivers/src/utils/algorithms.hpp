#pragma once

namespace utils
{
    /**
     * Euclidean algorithm to compute the greatest common divisor.
     */
    template<class T>
    T gcd(T a, T b) {
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
}
