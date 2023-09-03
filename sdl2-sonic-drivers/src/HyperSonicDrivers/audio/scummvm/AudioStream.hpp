#pragma once

#include <cstdint>

namespace HyperSonicDrivers::audio::scummvm
{
    /**
    * Generic audio input stream. Subclasses of this are used to feed arbitrary
    * sampled audio data into ScummVM's audio mixer.
    */
    class AudioStream
    {
    public:
        virtual ~AudioStream() {}

        /**
         * Fill the given buffer with up to @p numSamples samples.
         *
         * Data must be in native endianness, 16 bits per sample, signed. For stereo
         * stream, the buffer will be filled with interleaved left and right channel
         * samples, starting with the left sample. Furthermore, the samples in the
         * left and right are summed up. So if you request 4 samples from a stereo
         * stream, you will get a total of two left channel and two right channel
         * samples.
         *
         * @return The actual number of samples read, or -1 if a critical error occurred.
         *
         * @note You *must* check whether the returned value is less than what you requested.
         *       This indicates that the stream is fully used up.
         *
         */
        virtual int readBuffer(int16_t* buffer, const int numSamples) = 0;

        /** Check whether this is a stereo stream. */
        virtual bool isStereo() const = 0;

        /** Sample rate of the stream. */
        virtual int getRate() const = 0;

        /**
         * Check whether end of data has been reached.
         *
         * If this returns true, it indicates that at this time there is no data
         * available in the stream. However, there might be more data in the future.
         *
         * This is used by e.g. a rate converter to decide whether to keep on
         * converting data or to stop.
         */
        virtual bool endOfData() const = 0;

        /**
         * Check whether end of stream has been reached.
         *
         * If this returns true, it indicates that all data in this stream is used up
         * and no additional data will appear in it in the future.
         *
         * This is used by the mixer to decide whether a given stream shall be
         * removed from the list of active streams (and thus be destroyed).
         * By default, this maps to endOfData().
         */
        virtual bool endOfStream() const { return endOfData(); }
    };
}

