#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::utils
{
    TEST(Algorithms, ARRAYCLEAR)
    {
        const int size = 5;
        int buf[size];
        ARRAYCLEAR<int, size>(buf, 3);
        EXPECT_THAT(buf, ::testing::ElementsAre(3, 3, 3, 3, 3));
    }

    class VLQTest : public ::testing::TestWithParam<std::tuple<std::vector<uint8_t>, uint32_t, int>> {};
    TEST_P(VLQTest, variable_length_quantities)
    {
        auto values = std::get<0>(GetParam());
        uint32_t exp_value = std::get<1>(GetParam());
        int exp_reads = std::get<2>(GetParam());
        const uint8_t* buf = values.data();
        uint32_t value = 0;

        int reads = utils::decode_VLQ(buf, value);
        EXPECT_EQ(value, exp_value);
        EXPECT_EQ(reads, exp_reads);
    }
    INSTANTIATE_TEST_SUITE_P(
        decode_vlq,
        VLQTest,
        ::testing::Values(
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0 }, 0, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x40 }, 0x40, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x7F }, 0x7F, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0 }, 0x80, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0 }, 0x2000, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0x7F }, 0x3FFF, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0x80, 0 }, 0x4000, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0x80, 0 }, 0x100000, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0xFF, 0x7F }, 0x1FFFFF, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0x80, 0x80, 0 }, 0x200000, 4),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0x80, 0x80, 0 }, 0x8000000, 4),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0xFF, 0xFF, 0x7F }, 0x0FFFFFFF, 4),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0xFF, 0xFF, 0x7F }, 0x0FFFFFFF, 4)
        )
    );

    class XMIVLQTest : public ::testing::TestWithParam<std::tuple<std::vector<uint8_t>, uint32_t, int>> {};
    TEST_P(XMIVLQTest, xmi_variable_length_quantities)
    {
        auto values = std::get<0>(GetParam());
        uint32_t exp_value = std::get<1>(GetParam());
        int exp_reads = std::get<2>(GetParam());
        const uint8_t* buf = values.data();
        uint32_t value = 0;

        int reads = utils::decode_xmi_VLQ(buf, value);
        EXPECT_EQ(value, exp_value);
        EXPECT_EQ(reads, exp_reads);
    }

    INSTANTIATE_TEST_SUITE_P(
        decode_xmi_VLQ,
        XMIVLQTest,
        ::testing::Values(
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0, 0xFF }, 0, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x40, 0xFF }, 0x40, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x7F,0x01, 0xFF }, 0x7F + 0x01, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x7F, 0x22, 0xFF }, 0xA1, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x7F, 0x22, 0x80, 0xFF }, 0xA1, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 127, 3, 0xFF }, 130, 2)
        )
    );

    TEST(Algorithms, chars_vector_to_string)
    {
        std::vector<uint8_t> v{ 'T','E','S','T' };
        std::string str = utils::chars_vector_to_string(v);
        EXPECT_STRCASEEQ(str.c_str(), "TEST");
    }

    TEST_P(VLQTest, encode_variable_length_quantities)
    {
        // encode TODO
        /*auto values = std::get<0>(GetParam());
        uint32_t exp_value = std::get<1>(GetParam());
        int exp_reads = std::get<2>(GetParam());
        const uint8_t* buf = values.data();
        uint32_t value = 0;

        int reads = utils::decode_VLQ(buf, value);
        EXPECT_EQ(value, exp_value);
        EXPECT_EQ(reads, exp_reads);*/

   }

    TEST(Algorithms, decode_encode_same_input)
    {
        // TODO
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
