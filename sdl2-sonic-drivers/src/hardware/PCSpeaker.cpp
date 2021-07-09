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

        constexpr double pi2 = 2.0 * M_PI;
        constexpr double pi_coeff = static_cast<double>(std::numeric_limits<int8_t>::max());

        typedef std::function<int8_t(uint32_t, uint32_t)> generatorFunction;

        template<typename T> inline T CLIP(T v, T amin, T amax)
        {
            if (v < amin) {
                return amin;
            }

            if (v > amax) {
                return amax;
            }

            return v;
        }

        int8_t generateSquare(const uint32_t x, const uint32_t oscLength)
        {
            return (x < (oscLength / 2)) ?
                std::numeric_limits<int8_t>::max() :
                std::numeric_limits<int8_t>::min();
        }

        int8_t generatePulse(const uint32_t x, const uint32_t oscLength)
        {
            return (x < (oscLength / 2)) ?
                std::numeric_limits<int8_t>::max() :
                0;
        }

        int8_t generateSine(const uint32_t x, const uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }

            return static_cast<char>(CLIP<int16_t>(
                static_cast<uint16_t>(round(pi_coeff * sin(pi2 * x / oscLength))),
                std::numeric_limits<int8_t>::min(),
                std::numeric_limits<int8_t>::max()
                ));
        }

        int8_t generateSaw(const uint32_t x, const uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }

            return ((x * (std::numeric_limits<uint16_t>::max() / oscLength)) >> 8) - std::numeric_limits<int8_t>::max();
        }

        int8_t generateTriangle(const int32_t x, const uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }

            int y = ((x * (std::numeric_limits<uint16_t>::max() / (oscLength / 2))) >> 8) - std::numeric_limits<int8_t>::max();

            return (x <= (oscLength / 2)) ? y : (std::numeric_limits<uint8_t>::max() - y);
        }

        const generatorFunction generateWave[] = { 
            &generateSquare,
            &generateSine,
            &generateSaw,
            &generateTriangle,
            &generatePulse
        };

        // END Generators

        void PCSpeaker::callback(void* userdata, _In_ uint8_t* audiobuf, int len)
        {
            PCSpeaker* self = reinterpret_cast<PCSpeaker*>(userdata);
            int16_t* buf = reinterpret_cast<int16_t*>(audiobuf);
            // divide by 2 because is a int16_t instead of uint8_t
            self->readBuffer(buf, len / 2);
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

        int PCSpeaker::getRate() const noexcept
        {
            return _rate;
        }
    }
} // End of namespace Audio
