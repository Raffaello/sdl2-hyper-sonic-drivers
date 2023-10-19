#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <std/eChannelGroupFormatter.hpp>

namespace std
{
    TEST(eChannelGroupFormatter, Music)
    {
        ASSERT_STRCASEEQ(std::format("{}", HyperSonicDrivers::audio::mixer::eChannelGroup::Music).c_str(), "Music");
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
