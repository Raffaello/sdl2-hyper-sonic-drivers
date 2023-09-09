#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/utils/endianness.hpp>
#include <exception>
#include <filesystem>
//#ifdef __GNUC__
#include <cstring>
//#endif

namespace HyperSonicDrivers::files
{
    using std::string;
    using std::fstream;

    File::File(const string& filename) : _filename(filename)
    {
        //_file.exceptions(fstream::eofbit | fstream::failbit | fstream::badbit);
        _file.open(filename, fstream::in | fstream::binary);
        if (!_file.is_open()) {
            throw std::system_error(errno, std::system_category(), "Cannot open file: " + _filename);
        }
    }

    uintmax_t File::size() const noexcept
    {
        return std::filesystem::file_size(_filename);
    }

    std::streampos File::tell() const noexcept
    {
        return _file.tellg();
    }

    void File::seek(const std::streamoff offs, const std::fstream::seekdir whence) const
    {
        _file.seekg(offs, whence);
        if (!_file.good()) {
            throw std::system_error(errno, std::system_category(), "Cannot seek file: " + _filename);
        }
    }

    void File::read(void* buf, std::streamsize size) const
    {
        if (!_file.read(reinterpret_cast<char*>(buf), size)) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + _filename + " (" + strerror(errno) + ")");
        }
    }

    void File::close() noexcept
    {
        if (_file.is_open()) {
            _file.close();
        }
    }

    std::string File::_readStringFromFile() const noexcept
    {
        string filename;
        char c = -1;

        while (_file.good() && c != 0) {
            c = _file.get();
            filename += c;
        }

        // removing the last c==0 inserted before stop the loop.
        filename.pop_back();

        return filename;
    }


    uint16_t File::readLE16() const noexcept
    {
        return utils::swapLE16(read<int16_t>());
    }

    uint32_t File::readLE32() const noexcept
    {
        return utils::swapLE32(read<int32_t>());
    }

    uint8_t File::readU8() const noexcept
    {
        return read<uint8_t>();
    }

    uint32_t File::readBE32() const noexcept
    {
        return utils::swapBE32(read<int32_t>());
    }

    uint32_t File::readBE16() const noexcept
    {
        return utils::swapBE16(read<int16_t>());
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
