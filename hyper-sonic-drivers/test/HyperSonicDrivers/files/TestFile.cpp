#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/files/File.hpp>
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

    TEST(File, invalid_argument)
    {
        EXPECT_THROW(File f(""), std::system_error);
    }

    TEST(File, file_not_found)
    {
        EXPECT_THROW(File f("nofile.nofile"), std::system_error);
    }

    class FileMock : public File
    {
    public:
        FileMock(const std::string& filename) : File(filename) {}
        std::string readStringFromFile() const
        {
            return readStringFromFile_();
        }
    };

    TEST(File, readEmptyString)
    {
        FileMock f("../fixtures/empty.zero");

        std::string s = "test";
        EXPECT_NO_THROW(s = f.readStringFromFile());
        EXPECT_TRUE(s.empty());
    }

    TEST(File, readString_EOF)
    {
        FileMock f("../fixtures/empty.empty");

        EXPECT_THROW(f.readStringFromFile(), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    _argc = argc;
    _argv = argv;

    return RUN_ALL_TESTS();
}
