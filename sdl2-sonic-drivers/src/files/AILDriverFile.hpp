#pragma once

#include <files/File.hpp>

namespace files
{
    class AILDriverFile : public File
    {
    public:
        AILDriverFile(const std::string& filename);
        virtual ~AILDriverFile();
    };
}
