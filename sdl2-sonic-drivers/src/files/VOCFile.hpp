#pragma once

#include <files/File.hpp>
#include <string>

namespace files
{
    class VOCFile : public File
    {
    public:
        VOCFile(const std::string& filename);
        virtual ~VOCFile();
    };
}
