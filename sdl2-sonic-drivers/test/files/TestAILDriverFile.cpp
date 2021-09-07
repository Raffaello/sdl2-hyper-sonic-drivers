#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/AILDriverFile.hpp>

namespace files
{
    TEST(AILDriverFile, ADLIB_ADV)
    {
        AILDriverFile drv("fixtures/ADLIB.ADV");
        EXPECT_EQ(drv.getMinimumApiVersion(), 200);
        EXPECT_EQ(drv.getDriverType(), static_cast<int>(AILDriverFile::eDriverType::XMIDI_EMULATION));
        EXPECT_STRCASEEQ(drv.getDataSuffix(), "AD");
        EXPECT_EQ(drv.getDeviceName_o(), "Ad Lib(R) Music Synthesizer Card");
        EXPECT_EQ(drv.getDeviceName_s(), "");
        EXPECT_EQ(drv.getServiceRate(), 120);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
