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

namespace hardware
{
    using softsynths::generators::generateWave;

    void PCSpeaker::callback(void* userdata, _In_ uint8_t* audiobuf, int len)
    {
        PCSpeaker* self = reinterpret_cast<PCSpeaker*>(userdata);
        switch (self->getBits())
        {
        case 8:
            self->readBuffer<int8_t>(reinterpret_cast<int8_t*>(audiobuf), len);
            break;
        case 16:
            self->readBuffer<int16_t>(reinterpret_cast<int16_t*>(audiobuf), len / 2);
            break;
        default:
            // not implemented
            return;
        }
    }

    PCSpeaker::PCSpeaker(const int32_t rate, const int8_t channels, const int8_t bits) :
        _rate(rate), _channels(channels), _bits(bits)
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

    uint32_t PCSpeaker::getRate() const noexcept
    {
        return _rate;
    }

    uint8_t PCSpeaker::getChannels() const noexcept
    {
        return _channels;
    }

    uint8_t PCSpeaker::getBits() const noexcept
    {
        return _bits;
    }
} // namespace hardware
