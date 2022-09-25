#pragma once

#include <string>
#include <stdexcept>
#include <fstream>

namespace files
{
    class File
    {
    public:
        File() = delete;
        File(const File&) = delete;
        File(const File&&) = delete;
        File& operator=(const  File&) = delete;

        File(const std::string& filename);
        virtual ~File() noexcept;

        uintmax_t size() const noexcept;
        std::streampos tell() noexcept;
        void seek(const std::streamoff offs, const std::fstream::seekdir whence = std::fstream::beg);
        void read(void* buf, std::streamsize size);
        void close() noexcept;

    protected:
        const std::string _filename;
        std::string _readStringFromFile() noexcept;

        uint16_t readLE16() noexcept;
        uint32_t readLE32() noexcept;
        uint8_t  readU8() noexcept;
        uint32_t readBE32() noexcept;
        uint32_t readBE16() noexcept;

        std::string _getFilename() const noexcept;
        std::string _getPath() const noexcept;
        void _assertValid(const bool expr) const;

    private:
        std::fstream  _file;

        template<typename T> T read();
    };

    template<typename T> T File::read()
    {
        T i;

        if (!_file.read(reinterpret_cast<char*>(&i), sizeof(T))) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + _filename);
        }

        return i;
    }
}
