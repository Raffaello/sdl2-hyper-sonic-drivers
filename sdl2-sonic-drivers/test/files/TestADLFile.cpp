#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/ADLFile.hpp>

namespace files
{
    TEST(ADLFile, cstorDefault)
    {
       
    }

    TEST(ADLFile, file_not_valid)
    {
        EXPECT_THROW(ADLFile f("TestFile.exe"), std::invalid_argument);
    }

    TEST(ADLFile, file_not_found)
    {
        EXPECT_THROW(ADLFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
