#pragma once
#include <cstdint>
#include <mutex>
#include <functional>
#ifdef __GNUC__
#define _In_
#endif

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

// TODO move to hardware, pcspeaker is hardware
//      audio/softsynths could be the generators.
namespace audio
{
    namespace softsynths
    {
        class PCSpeaker final
        {
        public:
            enum class eWaveForm
            {
                SQUARE = 0,
                SINE,
                SAW,
                TRIANGLE,
                PULSE
            };

            static void callback(void* userdata, _In_ uint8_t* audiobuf, int len);

            PCSpeaker(const int rate = 44100, const int audio_channels = 2);
            ~PCSpeaker();

            // atomic?
            uint8_t volume = 128;

            // if length == -1 then infinte playing
            void play(const eWaveForm wave, const int freq, const int32_t length);
            /** Stop the currently playing note after delay ms. */
            void stop(const int32_t delay = 0);
            bool isPlaying() const noexcept;
            int readBuffer(int16_t* buffer, const int numSamples);
            int getRate() const noexcept;
        private:
            std::mutex _mutex;
            
            eWaveForm _wave = eWaveForm::SQUARE;
            
            const int _rate;
            const uint8_t _channels;
            
            bool _loop = false;
            uint32_t _oscLength = 0;
            uint32_t _oscSamples = 0;
            uint32_t _remainingSamples = 0;
        };
    }
} // End of namespace Audio
    