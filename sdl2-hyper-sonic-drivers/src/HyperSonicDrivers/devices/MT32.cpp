#include <HyperSonicDrivers/devices/MT32.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::devices
{
    // test
    //static audio::midi::MIDIEvent MT32SysEx(uint32_t addr, const uint8_t* data, uint32_t dataSize)
    //{
    //    //typedef uint8_t byte;
    //    //uint32_t dataSize = 0;
    //    //uint8_t* data = nullptr;
    //    //uint32_t addr = 0x1FC000;
    //    static const uint8_t header[] = { 0x41, 0x10, 0x16, 0x12 };

    //    uint8_t* msg = new uint8_t[sizeof(header) + 4 + dataSize];
    //    memcpy(msg, header, sizeof(header));
    //    uint8_t* dst = msg + sizeof(header);
    //    const uint8_t* src = dst;

    //    *dst++ = (addr >> 14) & 0x7F;
    //    *dst++ = (addr >> 7) & 0x7F;
    //    *dst++ = addr & 0x7F;

    //    while (dataSize) {
    //        *dst++ = *data++;
    //        --dataSize;
    //    }

    //    uint8_t checkSum = 0;
    //    while (src < dst)
    //        checkSum -= *src++;

    //    *dst++ = checkSum & 0x7F;

    //    dataSize = dst - msg;
    //    audio::midi::MIDIEvent e;
    //    for (int i = 0; i < sizeof(header) + 4 + dataSize; i++)
    //        e.data.push_back(msg[i]);

    //    delete[] msg;

    //    return e;
    //}


    MT32::MT32(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::filesystem::path& control_rom_file,
        const std::filesystem::path& pcm_rom_file) :
        IDevice(mixer, eDeviceType::Mt32)
    {
        m_mt32 = std::make_shared<hardware::mt32::MT32>(control_rom_file, pcm_rom_file, mixer);
        m_hardware = m_mt32.get();

        //m_mt32->start(nullptr);

        // test

        // reset MT32
        //m_mt32->m_service.playSysex(MT32SysEx(0x1FC000, 0, 0));
       // utils::delayMillis(250);

        // Setup master tune, reverb mode, reverb time, reverb level, channel mapping, partial reserve and master volume
        //static const uint8_t initSysex1[] = "\x40\x00\x04\x04\x04\x04\x04\x04\x04\x04\x04\x04\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x64";
        //sendSysEx(MT32SysEx(0x40000, initSysex1, sizeof(initSysex1) - 1));
        //utils::delayMillis(40);
        // Map percussion to notes 24 - 34 without reverb. It still happens in the DOTT driver, but not in the SAMNMAX one.
        //static const uint8_t initSysex2[] = "\x40\x64\x07\x00\x4a\x64\x06\x00\x41\x64\x07\x00\x4b\x64\x08\x00\x45\x64\x06\x00\x44\x64"
        //    "\x0b\x00\x51\x64\x05\x00\x43\x64\x08\x00\x50\x64\x07\x00\x42\x64\x03\x00\x4c\x64\x07\x00";
        //sendSysEx(MT32SysEx(0xC090, initSysex2, sizeof(initSysex2) - 1));
        //utils::delayMillis(40);


        //const uint8_t pbRange = 0x10;
        //for (int i = 0; i < 128; ++i) {
        //    //sendSysEx(MT32SysEx(0x014004 + (i << 3), &pbRange, 1));
        //    utils::delayMillis(5);
        //}
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
}
