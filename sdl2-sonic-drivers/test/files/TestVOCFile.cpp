#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/VOCFile.hpp>
#include <cstdint>

namespace files
{
    TEST(VOCFile, cstorDefault)
    {
        VOCFile f("fixtures/VSCREAM1.VOC");

        EXPECT_STRCASEEQ(f.getVersion().c_str(), "1.10");
        EXPECT_EQ(f.getChannels(), 1);
        EXPECT_EQ(f.getSampleRate(), 8000);
        EXPECT_EQ(f.getDataSize(), 5817);
        EXPECT_EQ(f.getData()[0], 0x80);
    }

    TEST(VOCFile, file_not_found)
    {
        EXPECT_THROW(VOCFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
