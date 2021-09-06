#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drivers/miles/XMidi.hpp>

namespace drivers
{
    namespace miles
    {
        TEST(XMidi, test)
        {
            XMidi xmidi;

            xmidi.readDriver("fixtures/");
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
