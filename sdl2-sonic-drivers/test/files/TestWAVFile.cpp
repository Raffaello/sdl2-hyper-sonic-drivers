#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/WAVFile.hpp>

namespace files
{
    TEST(WAVFile, cstorDefault)
    {
        WAVFile f("fixtures/Wav_868kb.wav");
        
    }

    TEST(WAVFile, file_not_found)
    {
        EXPECT_THROW(WAVFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
