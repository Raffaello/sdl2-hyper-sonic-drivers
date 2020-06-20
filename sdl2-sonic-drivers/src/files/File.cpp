#include "File.hpp"
#include <filesystem>

using std::string;

File::File(const string& filename): _filename(filename)
{
    _file = SDL_RWFromFile(filename.c_str(), "rb");

    if (_file == nullptr) {
        std::string e = SDL_GetError();
        throw std::invalid_argument("Cannot open file: " + filename + " -- ERROR:" + e);
    }
}

File::~File()
{
    if (_file != nullptr) {
        SDL_RWclose(_file);
    }
}

uint64_t File::tell() const noexcept
{
    return SDL_RWtell(_getFile());
}

int64_t File::size() const noexcept
{
    return SDL_RWsize(_getFile());
}

void File::seek(const int64_t offs, const int whence) const
{
    if (SDL_RWseek(_getFile(), offs, whence) == -1) {
        throw std::runtime_error(SDL_GetError());
    }
}

void File::readOnce(void* buf, size_t size) const
{
    _read(buf, size, 1);
}

std::string File::_readStringFromFile() const
{
    string filename;
    char c;

    do {
        c = SDL_ReadU8(_file);
        filename += c;
    } while (c != 0);
    filename.erase(filename.length() - 1);

    return filename;
}

void File::_read(void* buf, size_t size, size_t maxnum) const
{
    if (SDL_RWread(_getFile(), buf, size, maxnum) != maxnum) {
        throw std::runtime_error(SDL_GetError());
    }
}

uint16_t File::readLE16() const
{
    return SDL_ReadLE16(_getFile());
}

uint32_t File::readLE32() const
{
    return SDL_ReadLE32(_getFile());
}

uint8_t File::readU8() const
{
    return SDL_ReadU8(_getFile());
}

uint32_t File::readBE32() const
{
    return SDL_ReadBE32(_getFile());
}

std::string File::_getFilename() const noexcept
{
    return std::filesystem::path(_filename).filename().string();
}

std::string File::_getPath() const noexcept
{
    return std::filesystem::path(_filename).parent_path().string();
}

void File::_assertValid(const bool expr) const
{
    if (!expr) {
        std::string str = std::string("Not a valid file: ") + std::string(typeid(*this).name());
        throw std::invalid_argument(str.c_str());
    }
}
