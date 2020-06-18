#define _USE_MATH_DEFINES
#include "PCSpeaker.hpp"
#include <cmath>
#include <cassert>
#include <cstring>

template<typename T> inline T CLIP(T v, T amin, T amax)
{
    if (v < amin) return amin; else if (v > amax) return amax; else return v;
}

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

namespace audio
{
    namespace softsynths
    {
        typedef std::function<char(uint32_t, uint32_t)> generatorFunction;

        char generateSquare(uint32_t x, uint32_t oscLength)
        {
            return (x < (oscLength / 2)) ? 127 : -128;
        }

        char generatePulse(uint32_t x, uint32_t oscLength)
        {
            return (x < (oscLength / 2)) ? 127 : 0;
        }

        char generateSine(uint32_t x, uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }
            double y = 128.0 * sin(2.0 * M_PI * x / oscLength);
            return static_cast<char>(CLIP<int16_t>(static_cast<int16_t>(round(y)), -128, 127));
        }

        char generateSaw(uint32_t x, uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }

            return ((x * (65536 / oscLength)) >> 8) - 128;
        }

        char generateTriangle(uint32_t x, uint32_t oscLength)
        {
            if (oscLength == 0) {
                return 0;
            }

            int y = ((x * (65536 / (oscLength / 2))) >> 8) - 128;

            return (x <= (oscLength / 2)) ? y : (256 - y);
        }

        const generatorFunction generateWave[] = { 
            &generateSquare,
            &generateSine,
            &generateSaw,
            &generateTriangle,
            &generatePulse
        };

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
                _remainingSamples = (_rate * length) / 1000;
                _loop = false;
            }

            //_mixedSamples = 0;
        }

        void PCSpeaker::stop(const int32_t delay)
        {
            std::lock_guard lck(_mutex);

            _remainingSamples = (_rate * delay) / 1000;
            _loop = false;
        }

        bool PCSpeaker::isPlaying() const noexcept
        {
            return _remainingSamples != 0;
        }

        int PCSpeaker::readBuffer(int16_t* buffer, const int numSamples)
        {
            std::lock_guard lck(_mutex);
            //assert(numSamples % _channels == 0);
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
                
                //_mixedSamples++;
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
