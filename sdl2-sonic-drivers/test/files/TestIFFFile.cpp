#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/IFFFile.hpp>
#include <string>

namespace files
{
    TEST(IFFFile, cstorDefault)
    {
        IFFFile f("TestIFFFile.exe");
        EXPECT_EQ(f.tell(), 0);
        f.close();
    }

    TEST(IFFFile, file_not_found)
    {
        EXPECT_THROW(IFFFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
