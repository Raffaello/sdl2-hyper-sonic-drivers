#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/PCSpeaker.hpp>

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
        EXPECT_EQ(ns, numSamples);
    }

    TEST(PCSPeaker, duration)
    {
        int16_t buf[1024];
        uint32_t ns = 100;
        int32_t time = 1000;
        PCSpeaker pcSpeaker(44100, 2);
        
        pcSpeaker.play(PCSpeaker::eWaveForm::SINE, 440, time);
        double tsum = 0.0;
        while (pcSpeaker.isPlaying()) {
            auto d = pcSpeaker.readBuffer<int16_t>(buf, ns);
            double t = (double)d*1000.0 / (double)pcSpeaker.getChannels() / (double)pcSpeaker.getRate();
            tsum += t;
        }
        EXPECT_EQ(time, tsum);
        //EXPECT_NEAR(elapsed_seconds.count(), 10.0, 0.5);
        //EXPECT_EQ(start, stop);
    }
}
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
