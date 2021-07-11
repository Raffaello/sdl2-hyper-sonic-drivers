/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

// TODO: replace M_PI with c++20 std::numbers::pi
#define _USE_MATH_DEFINES
#include "PCSpeaker.hpp"
#include <cmath>
#include <cassert>
#include <cstring>
#include <limits>

namespace audio
{
    namespace hardware
    {
        // Generators
        // TODO: move generators to softsynths/generators namespace ?
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
            return CLIP<T> (
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

            constexpr double pi2 = 2.0 * M_PI;
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

        // END Generators

        void PCSpeaker::callback16bits(void* userdata, _In_ uint8_t* audiobuf, int len)
        {
            PCSpeaker* self = reinterpret_cast<PCSpeaker*>(userdata);
            int16_t* buf = reinterpret_cast<int16_t*>(audiobuf);
            // divide by 2 because is a int16_t instead of uint8_t
            self->readBuffer(buf, len / 2);
        }

        void PCSpeaker::callback8bits(void* userdata, _In_ uint8_t* audiobuf, int len)
        {
            PCSpeaker* self = reinterpret_cast<PCSpeaker*>(userdata);
            int8_t* buf = reinterpret_cast<int8_t*>(audiobuf);
            self->readBuffer8bits(buf, len);
        }

        PCSpeaker::PCSpeaker(const int rate, const int channels) : _rate(rate), _channels(channels)
        {
        }

        PCSpeaker::~PCSpeaker()
        {
        }

        void PCSpeaker::play(const eWaveForm wave, const int freq, const int32_t length)
        {
            std::lock_guard lck(_mutex);

            _wave = wave;
            _oscLength = _rate / freq;
            _oscSamples = 0;
            if (length == -1) {
                _remainingSamples = 1;
                _loop = true;
            }
            else {
                _setRemainingSamples(length);
            }
        }

        void PCSpeaker::stop(const int32_t delay)
        {
            std::lock_guard lck(_mutex);
            _setRemainingSamples(delay);
        }

        bool PCSpeaker::isPlaying() const noexcept
        {
            return _remainingSamples != 0;
        }

        int PCSpeaker::readBuffer(int16_t* buffer, const int numSamples)
        {
            std::lock_guard lck(_mutex);
            int i;
            for (i = 0; _remainingSamples && (i < numSamples); i++) {
                int16_t v = generateWave[static_cast<int>(_wave)](_oscSamples, _oscLength) * volume;

                for (int j = 0; j < _channels; j++, i++) {
                    buffer[i] = v;
                }

                if (++_oscSamples >= _oscLength) {
                    _oscSamples = 0;
                }

                if (!_loop) {
                    _remainingSamples--;
                }
            }
            
            // Clear the rest of the buffer
            if (i < numSamples) {
                memset(buffer + i, 0, (numSamples - i) * sizeof(int16_t));
            }

            return numSamples;
        }

        int PCSpeaker::readBuffer8bits(int8_t* buffer, const int numSamples)
        {
            std::lock_guard lck(_mutex);
            int i;
            for (i = 0; _remainingSamples && (i < numSamples); i++) {
                int8_t v = generateWave[static_cast<int>(_wave)](_oscSamples, _oscLength) * volume;

                for (int j = 0; j < _channels; j++, i++) {
                    buffer[i] = v;
                }

                if (++_oscSamples >= _oscLength) {
                    _oscSamples = 0;
                }

                if (!_loop) {
                    _remainingSamples--;
                }
            }

            // Clear the rest of the buffer
            if (i < numSamples) {
                memset(buffer + i, 0, (numSamples - i) * sizeof(int8_t));
            }

            return numSamples;
        }

        int PCSpeaker::getRate() const noexcept
        {
            return _rate;
        }
    }
} // End of namespace Audio
