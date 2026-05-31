#include <cstring>
#include <format>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    using utils::logW;

    AdLibChannel::AdLibChannel(const uint8_t channel) :
        IMidiChannel(channel)
    {
        memset(&_partInstr, 0, sizeof(_partInstr));
        memset(&_partInstrSecondary, 0, sizeof(_partInstrSecondary));
    }

    void AdLibChannel::setInstr(const bool isOpl3) noexcept
    {
        if (isOpl3)
        {
            memcpy(&_partInstr, &g_gmInstrumentsOPL3[program][0], sizeof(AdLibInstrument));
            memcpy(&_partInstrSecondary, &g_gmInstrumentsOPL3[program][1], sizeof(AdLibInstrument));
        }
        else
        {
            memcpy(&_partInstr, &g_gmInstruments[program], sizeof(AdLibInstrument));
        }
    }

    void AdLibChannel::setCustomInstr(const uint8_t* instr) noexcept
    {
        memcpy(&_partInstr, instr, sizeof(AdLibInstrument));
    }
}
