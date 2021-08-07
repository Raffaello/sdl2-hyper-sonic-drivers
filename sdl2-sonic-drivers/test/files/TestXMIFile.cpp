#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/XMIFile.hpp>
#include <string>

namespace files
{
    TEST(XMIFile, cstorDefault)
    {
        XMIFile f("fixtures/header.xmi");
        EXPECT_EQ(f.size(), 56);
        EXPECT_EQ(f.getNumTracks(), 1);
    }

    TEST(XMIFile, file_not_valid)
    {
        EXPECT_THROW(XMIFile f("TestFile.exe"), std::invalid_argument);
        
    }

    TEST(XMIFile, file_not_found)
    {
        EXPECT_THROW(XMIFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
