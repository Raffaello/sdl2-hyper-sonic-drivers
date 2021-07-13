#pragma once
#include <softsynths/generators/generators.hpp>
#include <cstdint>
#include <mutex>
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

// TODO
//      HARDWARE SOMEHOW WILL BE THE LOW LEVEL API OR NOT REALLY ACCESSIBLE
//      WHILE AUDIO IS THE INTERFACE HIGH LEVEL API TO MAKE SOUNDS
//      DRIVERS IS IN BETWEEN... ALSO MIGHT CONTAIN MILES SOUND DRIVERS (AIL)
//      AND READING FILE FORMATS LIKE ADL OR XMI
namespace audio
{
    namespace hardware
    {
        class PCSpeaker final
        {
        public:
            typedef softsynths::generators::eWaveForm eWaveForm;
            /// <summary>
            /// used for SDL_Mixer
            /// </summary>
            /// <param name="userdata"></param>
            /// <param name="audiobuf"></param>
            /// <param name="len"></param>
            static void callback16bits(void* userdata, _In_ uint8_t* audiobuf, int len);
            
            static void callback8bits(void* userdata, _In_ uint8_t* audiobuf, int len);

            PCSpeaker(const int rate = 44100, const int audio_channels = 2);
            ~PCSpeaker();

            std::atomic_int8_t volume = 128;

            /// <summary>
            /// Play a sound
            /// </summary>
            /// <param name="wave">wave form type</param>
            /// <param name="freq">Hz frequency</param>
            /// <param name="length">duration in ms, -1 infinite length</param>
            void play(const eWaveForm waveForm, const int freq, const int32_t length);
            /** Stop the currently playing note after delay ms. */
            void stop(const int32_t delay = 0);
            bool isPlaying() const noexcept;
            int readBuffer(int16_t* buffer, const int numSamples);
            int readBuffer8bits(int8_t* buffer, const int numSamples);
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

            inline void _setRemainingSamples(const int32_t length) noexcept {
                _remainingSamples = (_rate * length) / 1000;
                _loop = false;
            }
        };
    }
} // End of namespace Audio
