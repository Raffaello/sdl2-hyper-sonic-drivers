#pragma once

#include <files/File.hpp>
#include <string>

namespace files
{
    namespace dmx
    {
        class MUSFile : public File
        {
        public:
            MUSFile(const std::string& filename);
            virtual ~MUSFile() = default;
        };
    }
}
