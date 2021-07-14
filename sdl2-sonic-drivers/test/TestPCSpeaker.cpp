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

    TEST(PCSPeaker, duration8bit1Channels)
    {
        int8_t buf[1024];
        uint32_t ns = 100;
        int32_t time = 1000;
        uint32_t freq = 44100;
        uint8_t channels = 1;

        PCSpeaker pcSpeaker(freq, channels, 8);

        pcSpeaker.play(PCSpeaker::eWaveForm::SINE, 440, time);
        double tsum = 0.0;
        uint32_t dsum = 0;
        while (pcSpeaker.isPlaying()) {
            uint32_t d = pcSpeaker.readBuffer<int8_t>(buf, ns);
            dsum += d;
            double t = (double)d * 1000.0 / (double)pcSpeaker.getChannels() / (double)pcSpeaker.getRate();
            tsum += t;
        }
        EXPECT_NEAR((double)time, tsum, 0.1);
        EXPECT_EQ(dsum, freq * time * channels / 1000);
    }

    TEST(PCSPeaker, duration16bit1Channels)
    {
        
    }

    TEST(PCSPeaker, duration16bit2Channels)
    {
        
    }

    TEST(PCSPeaker, duration8bit2Channels)
    {
        
    }
}
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
