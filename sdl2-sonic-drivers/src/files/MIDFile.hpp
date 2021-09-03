#pragma once

#include <files/File.hpp>
#include <cstdint>

namespace files
{
    class MIDFile : public File
    {
    public:
        MIDFile(const std::string& filename);
        virtual ~MIDFile();
        /// <summary>
        /// Variable length quantity decoding algorithm
        /// </summary>
        /// <param name="encoded"></param>
        /// <returns></returns>
        //int32_t decode_VLQ(const int32_t encoded) const;

    private:
    };
}
