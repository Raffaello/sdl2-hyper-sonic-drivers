#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/PCSpeaker.hpp>
#include <iostream>

namespace hardware
{
    TEST(PCSpeaker, cstor)
    {
        PCSpeaker pcSpeaker(1, 2, 3);
        EXPECT_EQ(pcSpeaker.getRate(), 1);
        EXPECT_EQ(pcSpeaker.getChannels(), 2);
        EXPECT_EQ(pcSpeaker.getBits(), 3);
    }

    TEST(PCSpeaker, isPlaying)
    {
        PCSpeaker pcSpeaker(44100, 8);
        pcSpeaker.play(PCSpeaker::eWaveForm::SINE, 440, 1);
        EXPECT_TRUE(pcSpeaker.isPlaying());
    }

    TEST(PCSpeaker, readBuffer)
    {
        int16_t buf[1024];
        PCSpeaker pcSpeaker(44100, 2);
        pcSpeaker.play(PCSpeaker::eWaveForm::SINE, 440, 1);
        uint32_t ns = 100;
        uint32_t numSamples = pcSpeaker.readBuffer<int16_t>(buf, ns);
        EXPECT_EQ(44100 * 2 * 1 / 1000, numSamples);
    }

    template<class T> class Duration : public ::testing::TestWithParam<std::tuple<uint32_t, uint8_t>>
    {
    public:
        Duration()
        {
            freq = std::get<0>(GetParam());
            channels = std::get<1>(GetParam());
            pcSpeaker = new PCSpeaker(freq, channels, sizeof(T) * 8);
            readbuf_ = std::bind(&PCSpeaker::readBuffer<T>, pcSpeaker, buf, ns);
        }

        uint32_t readbuf()
        {
            return pcSpeaker->readBuffer<T>(buf, ns);
        }
    protected:
        PCSpeaker* pcSpeaker;
        std::function<uint32_t(T, const uint32_t)> readbuf_;
        T buf[1024];
        uint32_t ns = 100;
        int32_t time = 1000;
        uint32_t freq;
        uint8_t channels;
 
        void test_()
        {
            this->pcSpeaker->play(PCSpeaker::eWaveForm::SINE, 440, this->time);
            double tsum = 0.0;
            uint32_t dsum = 0;

            while (pcSpeaker->isPlaying())
            {
                uint32_t d = this->readbuf();
                dsum += d;
                double t = (double)d * 1000.0 / (double)this->pcSpeaker->getChannels() / (double)this->pcSpeaker->getRate();
                tsum += t;
            }

            EXPECT_NEAR((double)this->time, tsum, 0.1);
            EXPECT_EQ(dsum, this->freq * this->time * this->channels / 1000);
        }
    };

    class Duration8 : public Duration<int8_t> {};
    TEST_P(Duration8, duration_8bits)
    {
        this->test_();
    }
    
    class Duration16 : public Duration<int16_t> {};
    TEST_P(Duration16, duration_16bits)
    {
        this->test_();
    }
   
    template<typename T, std::size_t... I>
    auto values(T* t, std::index_sequence<I...>)
    {
        return ::testing::Values(t[I]...);
    }
   
    auto make_duration_values()
    {
        constexpr int nf = 8;
        constexpr int nc = 7;
        const int freq[nf] = { 11025, 22050, 44100, 48000, 88200, 96000, 176400, 192000 };
        const int channels[nc] = { 1, 2, 4, 5, 6, 7, 8 };
        std::tuple<int, int> res[nf * nc];
        int i = 0;
        for (auto& f : freq) {
            for (auto& c : channels) {
                res[i++] = std::make_tuple<>(f, c);
            }
        }

        return values(res, std::make_index_sequence<nf * nc>{});
    }

    INSTANTIATE_TEST_SUITE_P(
        PCSpeaker,
        Duration8,
        make_duration_values()
    );
    INSTANTIATE_TEST_SUITE_P(
        PCSpeaker,
        Duration16,
        make_duration_values()
    );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
