#include <HyperSonicDrivers/devices/MT32.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::devices
{
    MT32::MT32(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::filesystem::path& control_rom_file,
        const std::filesystem::path& pcm_rom_file) :
        IDevice(mixer, eDeviceType::Mt32)
    {
        m_mt32 = std::make_shared<hardware::mt32::MT32>(control_rom_file, pcm_rom_file, mixer);
        m_hardware = m_mt32.get();
    }

    bool MT32::init() noexcept
    {
        // TODO: it can be bring up into the parent class using IHardware check if not nullptr
        if (isInit())
            return true;

        m_init = m_mt32->init();

        return m_init;
    }

    bool MT32::shutdown() noexcept
    {
        // TODO: it can be bring up into the parent class using iHardware checkin if not nullptr
        m_mt32->stop();
        return false;
    }

    eDeviceName MT32::getName() const noexcept
    {
        return eDeviceName::Mt32;
    }

    void MT32::lcd_message(const std::string& msg) noexcept
    {
        m_mt32->sysEx(
            hardware::mt32::mt32_sysex_addr_LCD,
            std::bit_cast<const uint8_t*>(msg.c_str()),
            //reinterpret_cast<const uint8_t*>(msg.c_str()),
            static_cast<uint32_t>(msg.size())
        );
    }
}
