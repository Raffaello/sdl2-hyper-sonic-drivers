#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/File.hpp>
#include <string>

int _argc;
char** _argv;

namespace HyperSonicDrivers::files
{
    TEST(File, cstorDefault)
    {
        File f(_argv[0]);
        EXPECT_EQ(f.tell(), 0);
        f.close();
    }

    TEST(File, file_not_found)
    {
        EXPECT_THROW(File f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    _argc = argc;
    _argv = argv;

    return RUN_ALL_TESTS();
}
