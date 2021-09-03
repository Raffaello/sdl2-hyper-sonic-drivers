#include <files/MIDFile.hpp>

namespace files
{
    constexpr int32_t MAX_VRQ = 0xFFFFFFF; // 3.5 bytes = 27 bits

    MIDFile::MIDFile(const std::string& filename) : File(filename)
    {
        // TODO VLQ



    
    }

    MIDFile::~MIDFile()
    {}

    /*int32_t MIDFile::decode_VLQ(const int32_t encoded) const
    {
        union
        {
            int32_t v;
            int8_t a, b, c, d;
        } u;
        
        u.v = encoded;
        int32_t result = 0;
        
        u.a 

    }*/
}
