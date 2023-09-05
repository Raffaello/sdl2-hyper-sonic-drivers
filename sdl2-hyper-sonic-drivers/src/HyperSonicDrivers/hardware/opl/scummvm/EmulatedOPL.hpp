
#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/audio/scummvm/SoundHandle.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>


namespace HyperSonicDrivers::hardware
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
            class EmulatedOPL : public OPL, protected audio::IAudioStream
            {
            public:
                EmulatedOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
                virtual ~EmulatedOPL();

                inline bool isStereo() const noexcept override
                {
                    return type != OplType::OPL2;
                }

                // OPL API
                void setCallbackFrequency(int timerFrequency) override;
                std::shared_ptr<audio::scummvm::SoundHandle> getSoundHandle() const noexcept override;
                // AudioStream API
                size_t readBuffer(int16_t* buffer, const size_t numSamples) override;
                int getRate() const noexcept override;
                bool endOfData() const noexcept override;

                // TODO: this can be bring up to OPL interface
                std::shared_ptr<audio::scummvm::Mixer> getMixer() const noexcept;

            protected:
                std::shared_ptr<audio::scummvm::Mixer> _mixer;
                // OPL API
                void startCallbacks(int timerFrequency) override;
                void stopCallbacks() override;

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
            private:
                int _baseFreq = 0;

                int _nextTick = 0;
                int _samplesPerTick = 0;

                std::shared_ptr<audio::scummvm::SoundHandle> _handle;
            };
        }
    }
}
