
#pragma once

#include <cstdint>
#include <hardware/opl/OPL.hpp>
#include <audio/scummvm/AudioStream.hpp>
#include <audio/scummvm/SoundHandle.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            /**
             * An OPL that represents an emulated OPL.
             *
             * This will send callbacks based on the number of samples
             * decoded in readBuffer().
             */
            class EmulatedOPL : public OPL, protected audio::scummvm::AudioStream
            {
            public:
                EmulatedOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer);
                virtual ~EmulatedOPL();

                // OPL API
                void setCallbackFrequency(int timerFrequency);

                // AudioStream API
                int readBuffer(int16_t* buffer, const int numSamples);
                int getRate() const;
                bool endOfData() const noexcept;
                
                const std::shared_ptr<audio::scummvm::Mixer> getMixer();

            protected:
                std::shared_ptr<audio::scummvm::Mixer> _mixer;
                // OPL API
                void startCallbacks(int timerFrequency);
                void stopCallbacks();

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
                virtual void generateSamples(int16_t* buffer, int numSamples) = 0;
            private:
                int _baseFreq = 0;

                int _nextTick = 0;
                int _samplesPerTick = 0;

                // TODO use a shared ptr
                audio::scummvm::SoundHandle* _handle;
            };
        }
    }
}
