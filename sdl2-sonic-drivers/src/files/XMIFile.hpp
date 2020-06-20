#pragma once

#include "IFFFile.hpp"
#include <string>

namespace files
{
    class XMIFile final : public IFFFile
    {
    public:
        XMIFile(const std::string& filename);
        virtual ~XMIFile();

    private:
        int _songs;
    };
}
