#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/devices/midi/MidiScummVM.hpp>
#include <cassert>

namespace HyperSonicDrivers::devices::midi
{
    MidiScummVM::MidiScummVM(const std::shared_ptr<hardware::opl::OPL>& opl, const bool opl3mode,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan) : IMidiDevice()
    {
        _adlib = std::make_shared<drivers::midi::scummvm::MidiDriver_ADLIB>(opl, opl3mode);
        _adlib->open(group, volume, pan);
    }

    MidiScummVM::~MidiScummVM()
    {
        _adlib->close();
    }

    void MidiScummVM::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
    {
        _adlib->send(e.toUint32());
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="msg">Always 3 size, if it is 2 in size, 3rd value must be zero</param>
    /// <param name="size"></param>
    /// <returns></returns>
    void MidiScummVM::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
    {
        assert(size >= 2 && size <= 3);
        uint32_t b = msg[0] + (msg[1] << 8);

        if (size == 3)
            b += (msg[2] << 16);
        _adlib->send(b);
    }

    void MidiScummVM::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
    {
        //TODO..
    }

    void MidiScummVM::pause() const noexcept
    {
        // TODO
    }

    void MidiScummVM::resume() const noexcept
    {
        // TODO
    }
}
