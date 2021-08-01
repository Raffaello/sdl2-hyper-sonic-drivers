#pragma once

//#include <SDL2/SDL.h>
#include <string>
#include <stdexcept>
#include <fstream>
//#include <iostream>

class File
{
public:
    File(const std::string& filename);
    virtual ~File();

    uintmax_t size() const noexcept;
    std::streampos tell() noexcept;
    void seek(const std::streamoff offs, const std::fstream::_Seekdir whence = std::fstream::beg);
    void read(void* buf, std::streamsize size);

protected:
    const std::string _filename;
    std::string _readStringFromFile();
    
    uint16_t readLE16();
    uint32_t readLE32();
    uint8_t  readU8();
    uint32_t readBE32();

    std::string _getFilename() const noexcept;
    std::string _getPath() const noexcept;
    void _assertValid(const bool expr) const;
private:
    std::fstream  _file;
};
