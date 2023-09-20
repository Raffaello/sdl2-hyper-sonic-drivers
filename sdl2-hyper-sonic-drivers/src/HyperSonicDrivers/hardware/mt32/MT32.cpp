#include <HyperSonicDrivers/hardware/mt32/MT32.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/files/File.hpp>
#include <format>
#include <std/MT32EmuReturnCode.hpp>

namespace HyperSonicDrivers::hardware::mt32
{
    namespace fs = std::filesystem;

    MT32::MT32(const std::filesystem::path& control_rom, const std::filesystem::path& pcm_rom,
        const std::shared_ptr<audio::IMixer>& mixer) : m_mixer(mixer)
    {
        //files::File control_rom_file(control_rom.string());
        //files::File pcm_rom_file(pcm_rom.string());

        //m_control_rom_data_size = control_rom_file.size();
        //m_pcm_rom_data_size = pcm_rom_file.size();

        //m_control_rom_data = std::make_unique<uint8_t[]>(m_control_rom_data_size);
        //m_pcm_rom_data = std::make_unique<uint8_t[]>(m_pcm_rom_data_size);

        //control_rom_file.read(m_control_rom_data.get(), m_control_rom_data_size);
        //pcm_rom_file.read(m_control_rom_data.get(), m_pcm_rom_data_size);

        // TODO: do i need a report handler? i guess so for logging purposes
        m_service.createContext();

        utils::logI(std::format("MUNT version: {}", m_service.getLibraryVersionString()));

        mt32emu_return_code ret = m_service.addROMFile(control_rom.string().c_str());
        if(ret != MT32EMU_RC_ADDED_CONTROL_ROM)
            utils::throwLogC<std::runtime_error>(std::format("can't add control ROM data (ret_code: {})", ret));

        ret = m_service.addROMFile(pcm_rom.string().c_str());
        if (ret != MT32EMU_RC_ADDED_PCM_ROM)
        {
            utils::throwLogC<std::runtime_error>(std::format("can't add pcm ROM data (ret_code: {})", ret));
        }
        /*mt32emu_return_code ret = m_service.addROMData(m_control_rom_data.get(), m_control_rom_data_size);
        if (ret != MT32EMU_RC_ADDED_CONTROL_ROM)
        {
            utils::throwLogC<std::runtime_error>(std::format("can't add control ROM data (ret_code: {})", ret));
        }*/

        /*ret = m_service.addROMData(m_pcm_rom_data.get(), m_pcm_rom_data_size);
        if (ret != MT32EMU_RC_ADDED_PCM_ROM)
        {
            utils::throwLogC<std::runtime_error>(std::format("can't add pcm ROM data (ret_code: {})", ret));
        }*/

        mt32emu_rom_info info;
        m_service.getROMInfo(&info);
        utils::logI(std::format("Control ROM info: ID={}, desc={}, sha1={}", info.control_rom_id, info.control_rom_description, info.control_rom_sha1_digest));
        utils::logI(std::format("pcm     ROM info: ID={}, desc={}, sha1={}", info.pcm_rom_id, info.pcm_rom_description, info.pcm_rom_sha1_digest));
    }

    MT32::~MT32()
    {
        if (m_init)
        {
            m_service.closeSynth();
            m_init = false;
        }

        m_service.freeContext();
    }

    bool MT32::init()
    {
        if (m_init)
            return true;

        mt32emu_return_code ret = m_service.openSynth();
        if (ret != MT32EMU_RC_OK)
        {
            utils::logC(std::format("can't open MT32 emulation device (ret code: {})", ret));
            return false;
        }

        m_init = true;

        reset();
        return true;
    }

    void MT32::reset()
    {
        m_service.setOutputGain(1.0f);
        m_service.setReverbOutputGain(1.0f);
        m_service.selectRendererType(MT32Emu::RendererType_BIT16S);
        m_service.setMIDIDelayMode(MT32Emu::MIDIDelayMode_IMMEDIATE);

        // TODO: need a way to change this as user requests
        m_service.setSamplerateConversionQuality(MT32Emu::SamplerateConversionQuality_BEST);

        m_output_rate = m_service.getActualStereoOutputSamplerate();
    }
}
