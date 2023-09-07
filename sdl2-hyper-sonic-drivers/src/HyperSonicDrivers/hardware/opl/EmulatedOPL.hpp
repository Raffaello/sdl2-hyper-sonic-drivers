
#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>


namespace HyperSonicDrivers::hardware::opl
{
    /**
     * An OPL that represents an emulated OPL.
     *
     * This will send callbacks based on the number of samples
     * decoded in readBuffer().
     */
    class EmulatedOPL : public OPL
    {
    protected:
        class Stream : public audio::IAudioStream
        {
        private:
            EmulatedOPL* m_opl = nullptr;
            uint32_t m_nextTick = 0;
            const uint32_t m_samplesPerTick;
        public:
            const bool stereo;
            const uint32_t rate;

            Stream(EmulatedOPL* opl, const bool stereo, const uint32_t rate, const uint32_t samplesPerTick) :
                m_opl(opl), stereo(stereo), rate(rate), m_samplesPerTick(samplesPerTick) {};

            inline bool isStereo() const noexcept override { return stereo; }
            size_t readBuffer(int16_t* buffer, const size_t numSamples) override;
            uint32_t getRate() const noexcept override { return rate; };
            bool endOfData() const noexcept override { return false; };
        };

    public:
        EmulatedOPL(const OplType type, const std::shared_ptr<audio::IMixer>& mixer);
        virtual ~EmulatedOPL();

        // OPL API
        uint32_t setCallbackFrequency(int timerFrequency) override;
        
        // TODO: this can be in OPL interface
        std::shared_ptr<audio::IMixer> getMixer() const noexcept;

    protected:
        std::shared_ptr<audio::IMixer> m_mixer;
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
        std::optional<uint8_t> m_channel_id;
        std::shared_ptr<audio::IAudioStream> m_stream;
    };
}
