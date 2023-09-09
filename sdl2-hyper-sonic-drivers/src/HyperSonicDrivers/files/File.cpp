#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/utils/endianness.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

#include <exception>
#include <filesystem>
#include <cstring>
#include <format>

namespace HyperSonicDrivers::files
{
    using std::string;
    using std::fstream;

    File::File(const string& filename, const std::fstream::openmode mode) : m_filename(filename)
    {
        m_file.open(filename, mode);
        if (!m_file.is_open())
        {
            const std::string e = std::format("Can't read file: {} ({} - )[{}]", m_filename, errno, strerror(errno), std::system_category());
            utils::logC(e, utils::ILogger::eCategory::System);
            throw std::system_error(errno, std::system_category(), e);
        }
    }

    uintmax_t File::size() const noexcept
    {
        return std::filesystem::file_size(m_filename);
    }

    std::streampos File::tell() const noexcept
    {
        return m_file.tellg();
    }

    void File::seek(const std::streamoff offs, const std::fstream::seekdir whence) const
    {
        m_file.seekg(offs, whence);
        if (!m_file.good())
        {
            const std::string e = std::format("Can't read file: {} ({} - )[{}]", m_filename, errno, strerror(errno), std::system_category());
            utils::logC(e, utils::ILogger::eCategory::System);
            throw std::system_error(errno, std::system_category(), e);
        }
    }

    void File::read(void* buf, std::streamsize size) const
    {
        if (!m_file.read(reinterpret_cast<char*>(buf), size))
        {
            const std::string e = std::format("Can't read file: {} ({} - )[{}]", m_filename, errno, strerror(errno), std::system_category());
            utils::logC(e, utils::ILogger::eCategory::System);
            throw std::system_error(errno, std::system_category(), e);
        }
    }

    void File::close() noexcept
    {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    std::string File::_readStringFromFile() const noexcept
    {
        string filename;
        char c = -1;

        while (m_file.good() && c != 0) {
            c = m_file.get();
            filename += c;
        }

        // removing the last c==0 inserted before stop the loop.
        filename.pop_back();

        return filename;
    }

    uint8_t File::readU8() const noexcept
    {
        return read<uint8_t>();
    }

    uint16_t File::readLE16() const noexcept
    {
        return utils::swapLE16(read<int16_t>());
    }

    uint32_t File::readLE32() const noexcept
    {
        return utils::swapLE32(read<int32_t>());
    }

    uint32_t File::readBE16() const noexcept
    {
        return utils::swapBE16(read<int16_t>());
    }

    uint32_t File::readBE32() const noexcept
    {
        return utils::swapBE32(read<int32_t>());
    }

    void File::_write(const char* buf, const size_t size, const size_t maxnum)
    {
        _assertValid(buf != nullptr);
        m_file.write(buf, size);
        if (!m_file.good())
        {
            const std::string e = std::format("Can't write file: {} ({} - )[{}]", m_filename, errno, strerror(errno), std::system_category());
            utils::logC(e, utils::ILogger::eCategory::System);
            throw std::system_error(errno, std::system_category(), e);
        }
    }

    std::string File::_getFilename() const noexcept
    {
        return std::filesystem::path(m_filename).filename().string();
    }

    std::string File::_getPath() const noexcept
    {
        return std::filesystem::path(m_filename).parent_path().string();
    }

    void File::_assertValid(const bool expr) const
    {
        if (!expr)
        {
            utils::throwLogE<std::invalid_argument>(std::format("Not a valid file: {}", m_filename));
        }
    }
}
