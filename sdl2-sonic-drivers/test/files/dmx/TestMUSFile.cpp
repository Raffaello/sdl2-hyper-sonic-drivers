#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/dmx/MUSFile.hpp>

namespace files
{
    namespace dmx
    {
        TEST(MUSFile, cstorDefault)
        {
            MUSFile f("fixtures/D_E1M1.MUS");
            // TODO
        }

        TEST(MIDFile, file_not_found)
        {
            EXPECT_THROW(MUSFile f(""), std::system_error);
        }

        TEST(MIDFile, musfile_sample)
        {
            MUSFile f("fixtures/D_E1M1.MUS");
            // TODO
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
