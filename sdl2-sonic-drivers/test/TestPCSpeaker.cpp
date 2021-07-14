#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/PCSpeaker.hpp>

TEST(PCSpeaker, test)
{
    FAIL();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}