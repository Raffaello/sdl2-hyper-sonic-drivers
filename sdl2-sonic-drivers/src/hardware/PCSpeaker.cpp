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
#include "PCSpeaker.hpp"
#include <cmath>
#include <cassert>
#include <cstring>
#include <limits>
#include <functional>

namespace audio
{
    namespace hardware
    {
        using softsynths::generators::generateWave;

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
                int16_t v = generateWave<int8_t>(_wave, _oscSamples, _oscLength) * volume;

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
                // TODO: with volume is overflowing
                int8_t v = generateWave<int8_t>(_wave, _oscSamples, _oscLength) * volume;

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
