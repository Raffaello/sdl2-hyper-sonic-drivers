#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/dmx/OP2File.hpp>

namespace files
{
    namespace dmx
    {
        TEST(OP2File, cstorDefault)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            EXPECT_STRCASEEQ("Acoustic Grand Piano", f.getInstrumentName(0).c_str());
            EXPECT_STRCASEEQ("Open Triangle", f.getInstrumentName(OP2FILE_NUM_INSTRUMENTS - 1).c_str());
        }

        TEST(OP2File, file_not_found)
        {
            EXPECT_THROW(OP2File f(""), std::system_error);
        }

        TEST(OP2File, getInstruments_out_of_bound)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            EXPECT_THROW(f.getInstrument(255), std::out_of_range);
        }

        TEST(OP2File, getInstrumentsName_out_of_bound)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            EXPECT_THROW(f.getInstrumentName(255), std::out_of_range);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
