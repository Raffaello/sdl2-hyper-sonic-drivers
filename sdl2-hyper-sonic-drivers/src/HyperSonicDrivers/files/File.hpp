#pragma once

#include <string>
#include <stdexcept>
#include <fstream>

namespace HyperSonicDrivers::files
{
    class File
    {
    public:
        File() = delete;
        File(const File&) = delete;
        File(const File&&) = delete;
        File& operator=(const  File&) = delete;

        File(
            const std::string& filename,
            const std::fstream::openmode mode = std::fstream::in | std::fstream::binary);
        virtual ~File() noexcept = default;

        uintmax_t size() const noexcept;
        std::streampos tell() const noexcept;
        void seek(const std::streamoff offs, const std::fstream::seekdir whence = std::fstream::beg) const;
        void read(void* buf, std::streamsize size) const;
        
        uint8_t  readU8() const;
        uint16_t readLE16() const;
        uint32_t readLE32() const;
        uint32_t readBE16() const;
        uint32_t readBE32() const;

        void write(const char* buf, const size_t size);
        void close() noexcept;

        std::string getFilename() const noexcept;
        std::string getPath() const noexcept;

    protected:
        const std::string m_filename;
        std::string readStringFromFile_() const;
        void assertValid_(const bool expr) const;
        void throwCriticalSystemError_(const std::string& msg) const;

    private:
        mutable std::fstream  m_file;

        template<typename T> T read_() const;
    };

    template<typename T> T File::read_() const
    {
        T i;

        if (!m_file.read(reinterpret_cast<char*>(&i), sizeof(T))) {
            throw std::system_error(errno, std::system_category(), "Cannot read file: " + m_filename);
        }

        return i;
    }
}
