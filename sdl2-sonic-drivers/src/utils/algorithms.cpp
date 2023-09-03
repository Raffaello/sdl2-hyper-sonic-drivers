#include <utils/algorithms.hpp>
#include <stdexcept>

namespace utils
{
    int decode_VLQ(const uint8_t buf[], uint32_t& out_value)
    {
        int i = 0;
        uint8_t byte = 0;
        out_value = 0;
        do
        {
            if (i >= 4) {
                throw std::runtime_error("decode_VLQ: more than 32bits VLQ input");
            }

            byte = buf[i++];
            out_value = (out_value << 7) | (byte & 0x7F);
        } while (byte & 0x80);

        return i;
    }

    int decode_xmi_VLQ(const uint8_t buf[], uint32_t& out_value)
    {
        int i = 0;
        uint8_t byte = 0;
        out_value = 0;
        do
        {
            // 0xFFFF / 0x7F = 0x1FF = 511
            if (i >= 512) {
                throw std::runtime_error("decode_xmi_VLQ: more than 32bits VLQ input");
            }

            byte = buf[i++];
            if (byte & 0x80) {
                i--;
                break;
            }

            out_value += byte;
        } while (true);

        return i;
    }

    std::string chars_vector_to_string(const std::vector<uint8_t>::const_iterator& begin, const std::vector<uint8_t>::const_iterator& end)
    {
        std::string str(begin, end);
        str.shrink_to_fit();
        return str;
    }

    std::string chars_vector_to_string(const std::vector<uint8_t>& e)
    {
        return chars_vector_to_string(e.begin(), e.end());
    }

    std::string chars_vector_to_string_skip_first(const std::vector<uint8_t>& e)
    {
        return chars_vector_to_string(++(e.begin()), e.end());
    }
}
