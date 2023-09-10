#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/utils/endianness.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

#include <exception>
#include <filesystem>
#include <cstring>
#include <format>
#include <sstream>

namespace HyperSonicDrivers::files
{
    using std::string;
    using std::fstream;

    File::File(const string& filename, const std::fstream::openmode mode) : m_filename(filename)
    {
        m_file.open(filename, mode);
        if (!m_file.is_open())
        {
            throwCriticalSystemError_("Can't open file");
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
            throwCriticalSystemError_("Can't seek file");
        }
    }

    void File::read(void* buf, std::streamsize size) const
    {
        if (!m_file.read(reinterpret_cast<char*>(buf), size))
        {
            throwCriticalSystemError_("Can't read file");
        }
    }

    void File::close() noexcept
    {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    std::string File::readStringFromFile_() const noexcept
    {
        std::stringbuf sb;
        m_file.get(sb, '\0');
        m_file.get();//waster the delimeter char
        if (!m_file.good())
            throwCriticalSystemError_("Can't readStringFromFile");
        return sb.str();
    }

    uint8_t File::readU8() const
    {
        return read_<uint8_t>();
    }

    uint16_t File::readLE16() const
    {
        return utils::swapLE16(read_<int16_t>());
    }

    uint32_t File::readLE32() const
    {
        return utils::swapLE32(read_<int32_t>());
    }

    uint32_t File::readBE16() const
    {
        return utils::swapBE16(read_<int16_t>());
    }

    uint32_t File::readBE32() const
    {
        return utils::swapBE32(read_<int32_t>());
    }

    void File::write(const char* buf, const size_t size)
    {
        assertValid_(buf != nullptr);
        m_file.write(buf, size);
        if (!m_file.good())
        {
            throwCriticalSystemError_("Can't write file");
        }
    }

    std::string File::getFilename() const noexcept
    {
        return std::filesystem::path(m_filename).filename().string();
    }

    std::string File::getPath() const noexcept
    {
        return std::filesystem::path(m_filename).parent_path().string();
    }

    void File::assertValid_(const bool expr) const
    {
        if (!expr)
        {
            utils::throwLogE<std::invalid_argument>(std::format("Not a valid file: {}", m_filename));
        }
    }

    void File::throwCriticalSystemError_(const std::string& msg) const
    {
        const auto ec = std::error_condition(errno, std::system_category());
        const std::string e = std::format("{}: {} ({} - {})[{}: ({}) {}]",
            msg, m_filename,
            errno, strerror(errno),
            ec.category().name(), ec.value(), ec.message()
        );

        utils::logC(e, utils::ILogger::eCategory::System);
        throw std::system_error(errno, std::system_category(), e);
    }
}
