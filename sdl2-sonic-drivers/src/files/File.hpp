#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <stdexcept>

class File
{
public:
    File(const std::string& filename);
    virtual ~File();

    int64_t size() const noexcept;
    uint64_t tell() const noexcept;
    void seek(const int64_t offs, const int whence = RW_SEEK_SET) const;
    void readOnce(void* buf, size_t size) const;

protected:
    const std::string _filename;
    inline SDL_RWops* _getFile() const noexcept { return _file; }
    std::string _readStringFromFile() const;
    
    void _read(void* buf, size_t size, size_t maxnum) const;
    uint16_t readLE16() const;
    uint32_t readLE32() const;
    uint8_t  readU8() const;
    uint32_t readBE32() const;

    std::string _getFilename() const noexcept;
    std::string _getPath() const noexcept;
    void _assertValid(const bool expr) const;
private:
    SDL_RWops* _file = nullptr;
};
