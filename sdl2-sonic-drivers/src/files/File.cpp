#include "File.hpp"
#include <filesystem>
#include <exception>
#include <utils/endianness.hpp>

namespace files
{
    using std::string;
    using std::fstream;

    File::File(const string& filename) : _filename(filename)
    {
        _file.open(filename, fstream::in | fstream::binary);
        if (!_file.is_open()) {
            throw std::system_error(errno, std::system_category(), "Cannot open file: " + _filename);
        }
    }

    File::~File()
    {
        this->close();
    }

    std::streampos File::tell() noexcept
    {
        return _file.tellg();
    }

    uintmax_t File::size() const noexcept
    {
        return std::filesystem::file_size(_filename);
    }

    void File::seek(const std::streamoff offs, const std::fstream::_Seekdir whence)
    {
        _file.seekg(offs, whence);
        if (!_file.good()) {
            throw std::system_error(errno, std::system_category(), "Cannot seek file: " + _filename);
        }
    }

    void File::read(void* buf, std::streamsize size)
    {
        if (!_file.read(reinterpret_cast<char*>(buf), size)) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + _filename);
        }
    }

    void File::close()
    {
        if (_file.is_open()) {
            _file.close();
        }
    }

    std::string File::_readStringFromFile()
    {
        string filename;
        char c = -1;

        while (_file.good() && c != 0) {
            c = _file.get();
            filename += c;
        }

        return filename;
    }


    uint16_t File::readLE16()
    {
        int16_t i = 0;

        if (!_file.read(reinterpret_cast<char*>(&i), sizeof(int16_t))) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + _filename);
        }

        return utils::swapLE16(i);
    }

    uint32_t File::readLE32()
    {
        int32_t i = 0;

        if (!_file.read(reinterpret_cast<char*>(&i), sizeof(int32_t))) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + _filename);
        }

        return utils::swapLE32(i);
    }

    uint8_t File::readU8()
    {
        uint8_t i = 0;

        if (!_file.read(reinterpret_cast<char*>(&i), sizeof(uint8_t))) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + _filename);
        }

        return i;
    }

    uint32_t File::readBE32()
    {
        int32_t i = 0;

        if (!_file.read(reinterpret_cast<char*>(&i), sizeof(int32_t))) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + _filename);
        }

        return utils::swapBE32(i);
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
            std::string str = std::string("Not a valid file: ") + _filename + " (" + std::string(typeid(*this).name()) + ")";
            throw std::invalid_argument(str.c_str());
        }
    }
}
