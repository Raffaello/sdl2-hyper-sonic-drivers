#pragma once

#include <HyperSonicDrivers/softsynths/generators/generators.hpp>
#include <cstdint>
#include <mutex>
#include <cstring>
#ifdef __GNUC__
#define _In_
#endif


// TODO
//      HARDWARE SOMEHOW WILL BE THE LOW LEVEL API OR NOT REALLY ACCESSIBLE
//      WHILE AUDIO IS THE INTERFACE HIGH LEVEL API TO MAKE SOUNDS
//      DRIVERS IS IN BETWEEN... ALSO MIGHT CONTAIN MILES SOUND DRIVERS (AIL)
//      AND READING FILE FORMATS LIKE ADL OR XMI
// NOTE:
//      THIS CLASS IS sort of HIGH LEVEL, while generators is low level
//      PCSpeaker still miss to read the pc speaker files and drivers.

namespace HyperSonicDrivers::hardware
{
    class PCSpeaker final
    {
    public:
        typedef softsynths::generators::eWaveForm eWaveForm;

        /// <summary>
        /// used for SDL_Mixer
        /// TODO: should be moved in the audio namespace as interface for SDL2 not here.
        /// </summary>
        /// <param name="userdata"></param>
        /// <param name="audiobuf"></param>
        /// <param name="len"></param>
        static void callback(void* userdata, _In_ uint8_t* audiobuf, int len);

        //TODO encode in 1 int the bits (union) with signed/unsiged so can be do a single if/switch?
        PCSpeaker(const int32_t rate = 44100, const int8_t audio_channels = 2, const int8_t bits = 16, const bool signed_ = true);
        ~PCSpeaker();

        /// <summary>
        /// Play a sound
        /// </summary>
        /// <param name="wave">wave form type</param>
        /// <param name="freq">Hz frequency</param>
        /// <param name="length">duration in ms, -1 infinite length</param>
        void play(const eWaveForm waveForm, const int freq, const int32_t length);
        /** Stop the currently playing note after delay ms. */
        void stop(const int32_t delay = 0);
        bool isActive() const noexcept;
        template<typename T> uint32_t readBuffer(T* buffer, uint32_t numSamples);
        uint32_t getRate() const noexcept;
        uint8_t getChannels() const noexcept;
        uint8_t getBits() const noexcept;
        bool getSigned() const noexcept;
    private:
        std::mutex _mutex;

        eWaveForm _wave = eWaveForm::SQUARE;

        const uint32_t _rate;
        const uint8_t _channels;
        const uint8_t _bits;
        const bool _signed;

        bool _loop = false;
        uint32_t _oscLength = 0;
        uint32_t _oscSamples = 0;
        uint32_t _remainingSamples = 0;

        inline void _setRemainingSamples(const int32_t length) noexcept {
            _remainingSamples = (_rate * length) / 1000;
            _loop = false;
        }
    };

    template<typename T> uint32_t PCSpeaker::readBuffer(T* buffer, uint32_t numSamples)
    {
        static_assert(std::numeric_limits<T>::is_integer);
        std::lock_guard lck(_mutex);
        uint32_t i = 0;

        for (; (_remainingSamples > 0) && ( numSamples > 0); numSamples--)
        {
            T v = softsynths::generators::generateWave<T>(_wave, _oscSamples, _oscLength);

            for (int j = 0; j < _channels; j++) {
                buffer[i++] = v;
            }
 
            if (++_oscSamples >= _oscLength) {
                _oscSamples = 0;
            }

            if (!_loop) {
                _remainingSamples--;
            }
        }

        // Clear the rest of the buffer
        if (numSamples > 0) {
            std::memset(buffer + i, 0, static_cast<size_t>(numSamples) * _channels * sizeof(T));
        }

        return i;
    }
} // namesapce hardware
