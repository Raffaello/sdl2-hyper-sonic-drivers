#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>

namespace HyperSonicDrivers::hardware
{
    class IEmulatedHardware
    {
    public:
        IEmulatedHardware(IEmulatedHardware&) = delete;
        IEmulatedHardware(IEmulatedHardware&&) = delete;
        IEmulatedHardware& operator=(IEmulatedHardware&) = delete;

        IEmulatedHardware() = default;
        virtual ~IEmulatedHardware() = default;

    protected:
        /**
        * get the emulated generated audio stream
        **/
        virtual std::shared_ptr<audio::IAudioStream> getAudioStream() const noexcept = 0;

        /**
         * Read up to 'length' samples.
         *
         * Data will be in native endianess, 16 bit per sample, signed.
         * For stereo OPL, buffer will be filled with interleaved
         * left and right channel samples, starting with a left sample.
         * Furthermore, the samples in the left and right are summed up.
         * So if you request 4 samples from a stereo OPL, you will get
         * a total of two left channel and two right channel samples.
         */
        virtual void generateSamples(int16_t* buffer, const size_t length) noexcept = 0;
    };
}
