#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <std/ILoggerFormatter.hpp>

namespace std
{
    TEST(ILoggerFormatter, Music)
    {
        ASSERT_STRCASEEQ(std::format("{}", HyperSonicDrivers::utils::ILogger::eLevel::Critical).c_str(), "Critical");
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
